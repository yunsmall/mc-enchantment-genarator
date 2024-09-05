//
// Created by 91246 on 24-8-28.
//

#include "core/EnchGenMachine.h"

#include <algorithm>
#include <sstream>
#include <ranges>


namespace GenEnch {
    EnchGenMachine::EnchGenMachine(const GameVersion &version,
                                   const std::shared_ptr<std::vector<EnchType> > &enchant_types,
                                   const std::shared_ptr<std::map<std::string, EnchItem> > &enchant_items):
        current_version{version}, enchant_items(enchant_items), enchant_types(enchant_types) {
    }

    std::string EnchGenMachine::genCommand(bool with_slash,bool nbt_postposition) const {
        if (current_item != nullptr) {
            std::stringstream stringstream;
            stringstream << R"({Enchantments:[)";

            for (auto i = selected_enchantment_storage.begin(); i != selected_enchantment_storage.end(); i++) {
                auto basic_unit = std::format(R"({{id:{},lvl:{}}})", i->first, i->second);
                if (std::next(i) != selected_enchantment_storage.end()) {
                    basic_unit += ',';
                }
                stringstream << basic_unit;
            }
            stringstream << R"(]})";

            if(nbt_postposition) {
                return std::format("{}give @p {} 1 0 {}",with_slash?"/":"", target_in_game_name, stringstream.str());
            }
            else {
                return std::format("{}give @p {}{}",with_slash?"/":"", target_in_game_name, stringstream.str());
            }

        }
        return {};
    }

    void EnchGenMachine::changeCurrentItem(const std::string &item_name) {
        auto target_item = enchant_items->find(item_name);
        if (target_item != enchant_items->end()) {
            current_item = &target_item->second;
            findAndSetCorespondingEnch();
        } else {
            current_item = nullptr;
            clearItemEnchDatas();
        }
    }

    bool EnchGenMachine::trySettingInGameName(const std::string &chinese_name) {
        if (current_item == nullptr) {
            return false;
        }
        auto target_name = current_item->game_names.find(chinese_name);
        if (target_name == current_item->game_names.end()) {
            return false;
        }
        target_in_game_name = target_name->second;
        return true;
    }

    void EnchGenMachine::findAndSetCorespondingEnch() {
        if (current_item == nullptr) {
            clearItemEnchDatas();
        } else {

            auto target_ench_kind = findMaxSuitableObj<decltype(current_item->enchDatas)>(
                    current_version, current_item->enchDatas,
                    std::function([](std::decay_t<decltype(current_item->enchDatas)>::const_reference datas) {
                        return GameVersion(datas.first);
                    }));

            // //找到小于当前设置版本的最大可用版本
            // const EnchKindDatas* target_ench_kind=nullptr;
            // GameVersion tmp_max_version{0,0,0};
            // for(const auto& i:current_item->datas) {
            //     //当前遍历的版本
            //     GameVersion tmp_version(i.first);
            //     //由于会有个叫“通用”的键，且上述键为全0肯定<=当前遍历的键，因此下面if至少执行一次，用来更改target_ench_kind
            //     if(tmp_version<current_version&&tmp_max_version<=tmp_version) {
            //         tmp_max_version=tmp_version;
            //         target_ench_kind=&i.second;
            //     }
            // }
            //保险起见判断nullptr
            if (target_ench_kind != nullptr) {
                best_enchantments = std::ranges::to<decltype(best_enchantments)>(
                        target_ench_kind->second.best_enchantments | std::views::transform(
                                [this](const auto &pair) {
                                    return std::pair(
                                            pair.first, pair.second | std::views::filter([this](auto &str) {
                                                return canBeUsedInCurrentVersion(str);
                                            }) | std::ranges::to<std::vector<std::string> >());
                                }));
                suitable_enchantments = std::ranges::to<decltype(suitable_enchantments)>(
                        target_ench_kind->second.suitable_enchantments | std::views::filter([this](const auto &name) {
                            return canBeUsedInCurrentVersion(name);
                        }));
            } else {
                clearItemEnchDatas();
            }
        }

    }

    std::string EnchGenMachine::determainEnchName(const std::string &chinese_ench_name) const {
        auto target_ench = std::ranges::find_if(*enchant_types,
                                                [&](decltype(enchant_types)::element_type::const_reference t) {
                                                    return t.name == chinese_ench_name;
                                                });

        if (target_ench != enchant_types->end()) {
            auto &target_names = target_ench->game_priv_names;

            auto target_version_str = findMaxSuitableObj<std::decay_t<decltype(target_names)> >(
                    current_version, target_names,
                    std::function([](std::decay_t<decltype(target_names)>::const_reference t) {
                        return GameVersion(t.first);
                    }));


            // //找到小于当前设置版本的最大可用版本
            // const std::string* target_version_str=nullptr;
            // GameVersion tmp_max_version{0,0,0};
            // for(const auto& i:target_names) {
            //     //当前遍历的版本
            //     GameVersion tmp_version(i.first);
            //     //由于会有个叫“通用”的键，且上述键为全0肯定<=当前遍历的键，因此下面if至少执行一次，用来更改target_ench_kind
            //     if(tmp_version<current_version&&tmp_max_version<=tmp_version) {
            //         tmp_max_version=tmp_version;
            //         target_version_str=&i.first;
            //     }
            // }

            // auto target_name_pair=std::ranges::sort((*target_ench)|std::views::take_while([this](decltype(target_ench)::reference t) {
            //     return current_version>t.min_version;
            // }),std::greater<>())
            if (target_version_str) {
                return target_version_str->second;
            }
        }
        return {};
    }

    bool EnchGenMachine::forceAddEnch(const std::string &chinese_name, uint32_t lvl) {
        if (current_item == nullptr) {
            return false;
        }
        auto in_game_name = determainEnchName(chinese_name);
        if (in_game_name.empty()) {
            return false;
        }
        selected_enchantment_storage[in_game_name] = lvl;
        return true;
    }

    bool EnchGenMachine::tryAddEnch(const std::string &chinese_name, uint32_t lvl) {
        if (!std::ranges::contains(suitable_enchantments, chinese_name)) {
            return false;
        }
        return forceAddEnch(chinese_name, lvl);
    }

    const EnchType * EnchGenMachine::tryGetEnchType(const std::string &chinese_name) {
        auto found_ench=std::ranges::find_if(*enchant_types,[&](auto& ench_type) {
            return ench_type.name==chinese_name;
        });
        if(found_ench==enchant_types->end()) {
            return nullptr;
        }
        return &*found_ench;
    }

    // void EnchGenMachine::trySettingToPreinstallEnch(const std::string& name) {
    //     selected_enchantment_storage.clear();
    //     auto found_preinstall=best_enchantments.find(name);
    //     if(found_preinstall==best_enchantments.end()) {
    //         return;
    //     }
    //
    //     std::ranges::for_each(
    //             (*enchant_types) |
    //             std::views::filter(
    //                     [&](decltype(enchant_types)::element_type::const_reference elem) {
    //                         return std::ranges::contains(found_preinstall->second, elem.name);
    //                     }),
    //             [this](decltype(enchant_types)::element_type::const_reference elem) {
    //                 tryAddEnch(elem.name, elem.max_level);
    //             });
    // }

    void EnchGenMachine::clearItemEnchDatas() {
        best_enchantments.clear();
        suitable_enchantments.clear();
    }
} // GenEnch
