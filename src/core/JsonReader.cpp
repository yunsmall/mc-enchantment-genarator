//
// Created by 91246 on 24-8-28.
//

#include "core/JsonReader.h"

#include <iostream>
#include <fstream>



namespace GenEnch {

    EnchItem JsonReader::readEnchItem(const std::string& name,const nlohmann::json &ench_item_json) {
        EnchItem item(name);
        item.game_names=ench_item_json[ench_item_names_key].get<decltype(EnchItem::game_names)>();
        for(auto& version:ench_item_json[ench_item_key].items()) {
            SuitableEnchDatas enchDatas;
            enchDatas.suitable_enchantments=version.value()[ench_item_suitable_enchantment].get<decltype(SuitableEnchDatas::suitable_enchantments)>();
            enchDatas.best_enchantments=version.value()[ench_item_best_enchantment].get<decltype(SuitableEnchDatas::best_enchantments)>();
            item.enchDatas[version.key()]=enchDatas;
        }
        return item;
    }

    nlohmann::json JsonReader::toJson(const EnchItem &item) {
        nlohmann::json item_json;
        item_json[ench_item_names_key]=item.game_names;
        for(auto& version_data:item.enchDatas) {
            nlohmann::json one_version_data;
            one_version_data[ench_item_suitable_enchantment]=version_data.second.suitable_enchantments;
            one_version_data[ench_item_best_enchantment]=version_data.second.best_enchantments;

            item_json[ench_item_key][version_data.first]=one_version_data;
        }
        return item_json;
    }

    std::shared_ptr<std::map<std::string, EnchItem>> JsonReader::readItems(std::filesystem::path ench_item_path) {
        auto items=std::make_shared<std::map<std::string, EnchItem>>();

        if(!is_directory(ench_item_path)) {
            return std::move(items);
        }
        for(const auto& entry:std::filesystem::directory_iterator(ench_item_path)) {
            if(entry.path().extension()==".json") {
                auto item_name=entry.path().stem().string();
                nlohmann::json item_json;
                std::ifstream json_file(entry.path());
                json_file>>item_json;

                items->insert(std::pair(item_name,readEnchItem(item_name,item_json)));
            }
        }
        return std::move(items);
    }

    EnchType JsonReader::readEnchType(const nlohmann::json& ench_json) {
        EnchType a_type;

        a_type.name=ench_json[ench_type_name];

        a_type.max_level=ench_json[ench_type_max_level];

        auto min_version_str=ench_json[ench_type_min_version].get<std::string>();
        a_type.min_version.fromStr(min_version_str);

        a_type.game_priv_names=ench_json[ench_type_game_priv_names].get<std::map<std::string,std::string>>();

        return a_type;
    }

    std::shared_ptr<std::vector<EnchType>> JsonReader::readEnchDatas(std::filesystem::path ench_json_path) {
        nlohmann::json ench_json;
        auto all_enchs=std::make_shared<std::vector<EnchType>>();
        std::ifstream ifile(ench_json_path);
        if(ifile) {
            ifile>>ench_json;
            for(auto ench_type=0;ench_type<ench_json[ench_type_key].size();ench_type++) {
                all_enchs->emplace_back(readEnchType(ench_json[ench_type_key][ench_type]));
            }
        }

        return std::move(all_enchs);
    }

    nlohmann::json JsonReader::toJson(const std::vector<EnchType> &enchTypes) {
        nlohmann::json out_json;
        nlohmann::json type_array=nlohmann::json::array();
        for(auto& type:enchTypes) {
            nlohmann::json one_type;
            one_type[ench_type_name]=type.name;
            one_type[ench_type_max_level]=type.max_level;
            one_type[ench_type_min_version]=type.min_version.toString();
            one_type[ench_type_game_priv_names]=type.game_priv_names;
            type_array.push_back(one_type);
        }
        out_json[ench_type_key]=type_array;
        return out_json;
    }
} // GenEnch