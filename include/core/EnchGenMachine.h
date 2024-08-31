//
// Created by 91246 on 24-8-28.
//

#ifndef ENCHGENMACHINE_H
#define ENCHGENMACHINE_H

#include <memory>
#include <vector>
#include <functional>

#include "core/EnchType.h"
#include "core/EnchItem.h"
#include "core/GameVersion.h"

namespace GenEnch {

class EnchGenMachine {
public:
    explicit EnchGenMachine(const GameVersion& version,const std::shared_ptr<std::vector<EnchType>>& enchant_types,const std::shared_ptr<std::map<std::string, EnchItem>>& enchant_items);

    virtual ~EnchGenMachine()=default;

    /**
     * @brief 获取最终的give指令
     * @return
     */
    [[nodiscard]] std::string genCommand(bool with_slash=true,bool nbt_postposition=false) const;

    /**
     * @brief 更改当前物品，找不到会设置成nullptr，调用此函数会自动更改相应的附魔数据
     * @param item_name
     */
    void changeCurrentItem(const std::string& item_name);

    bool trySettingInGameName(const std::string& chinese_name);

    /**
         * @brief 判断当前版本能否用这个附魔
         * @param ench_chinese_name
         * @return
         */
    [[nodiscard]] bool canBeUsedInCurrentVersion(const std::string& ench_chinese_name) const {
        auto found_type=std::ranges::find_if(*enchant_types,[&](decltype(enchant_types)::element_type::const_reference elem) {
            return elem.name==ench_chinese_name;
        });
        if(found_type!=enchant_types->end()) {
            return canBeUsedInCurrentVersion(*found_type);
        }
        return false;
    }

    /**
     * @brief 判断当前版本能否用这个附魔
     * @param ench_type
     * @return
     */
    [[nodiscard]] bool canBeUsedInCurrentVersion(const EnchType& ench_type) const {
        return ench_type.min_version<=current_version;
    }

    /**
     * @brief 根据中文附魔名决定游戏中的NBT英文ID名，和当前物品无关
     * @param chinese_ench_name
     * @return
     */
    [[nodiscard]] std::string determainEnchName(const std::string& chinese_ench_name) const;

    [[nodiscard]] const decltype(SuitableEnchDatas::best_enchantments) & get_best_enchantments() const {
        return best_enchantments;
    }

    [[nodiscard]] const decltype(SuitableEnchDatas::suitable_enchantments) & get_suitable_enchantments() const {
        return suitable_enchantments;
    }
    bool forceAddEnch(const std::string& chinese_name,uint32_t lvl);
    bool tryAddEnch(const std::string& chinese_name,uint32_t lvl);

    const EnchType* tryGetEnchType(const std::string& chinese_name);
    // void trySettingToPreinstallEnch(const std::string& name);

    [[nodiscard]] const GameVersion & get_current_version() const {
        return current_version;
    }

    [[nodiscard]] const EnchItem * get_current_item() const {
        return current_item;
    }

    [[nodiscard]] std::string& get_target_in_game_name() {
        return target_in_game_name;
    }

    /**
     * @brief 储存最终的英文id和目标等级的map
     */
    std::map<std::string,uint32_t> selected_enchantment_storage;

    [[nodiscard]] std::shared_ptr<const std::map<std::string, EnchItem>> get_enchant_items() const {
        return enchant_items;
    }

    [[nodiscard]] std::shared_ptr<const std::vector<EnchType>> get_enchant_types() const {
        return enchant_types;
    }

private:

    void clearItemEnchDatas();

    /**
     * @brief 决定对应的适合附魔和最好附魔的附魔类型
     */
    void findAndSetCorespondingEnch();

    /**
     * @brief 返回所有数据中小于目标版本但最大的对象，照常理来说不会为nullptr，建议还是判断一下
     * @tparam Container 容器，在哪里找目标
     * @tparam T 目标类型
     * @param target_version 目标版本
     * @param container 哪个容器对象
     * @param howToGetVersion 怎么从目标中获取版本信息
     * @return
     */
    template<typename Container,typename T=decltype(*Container().begin())>
    const T* findMaxSuitableObj(const GameVersion& target_version,const Container& container,const std::function<GameVersion(const T&)>& howToGetVersion) const {

        //找到小于当前设置版本的最大可用版本
        const T* target=nullptr;
        GameVersion tmp_max_version{0,0,0};
        for(const auto& i:container) {
            //当前遍历的版本
            GameVersion tmp_version=howToGetVersion(i);
            //由于会有个叫“通用”的键，且上述键为全0肯定<=当前遍历的键，因此下面if至少执行一次，用来更改target
            if(tmp_version<current_version&&tmp_max_version<=tmp_version) {
                tmp_max_version=tmp_version;
                target=&i;
            }
        }
        return target;
    }

    GameVersion current_version;
    const EnchItem* current_item=nullptr;
    std::string target_in_game_name;

    /**
     * @brief 里面为中文名
     */
    decltype(SuitableEnchDatas::best_enchantments) best_enchantments;
    /**
     * @brief 里面为中文名
     */
    decltype(SuitableEnchDatas::suitable_enchantments) suitable_enchantments;

    const std::shared_ptr<std::map<std::string, EnchItem>> enchant_items;
    const std::shared_ptr<std::vector<EnchType>> enchant_types;
};

} // GenEnch

#endif //ENCHGENMACHINE_H
