//
// Created by 91246 on 24-8-28.
//

#ifndef JSONREADER_H
#define JSONREADER_H

#include <memory>
#include <vector>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "EnchType.h"
#include "EnchItem.h"
#include "GameVersion.h"

namespace GenEnch {

class JsonReader {
public:
    JsonReader() = default;

    virtual ~JsonReader() = default;

    constexpr static auto ench_type_key="附魔种类";
    constexpr static auto ench_type_name="名字";
    constexpr static auto ench_type_max_level="最大值";
    constexpr static auto ench_type_min_version="最低版本";
    constexpr static auto ench_type_game_priv_names="游戏内名称";
    constexpr static auto ench_type_game_priv_general_names="通用";

    std::shared_ptr<std::vector<EnchType>> readEnchDatas(std::filesystem::path ench_json_path);
    nlohmann::json toJson(const std::vector<EnchType> &enchTypes);

    constexpr static auto ench_item_names_key="可用名字";
    constexpr static auto ench_item_key="附魔数据";
    constexpr static auto ench_item_general_version="通用";
    constexpr static auto ench_item_suitable_enchantment="适用附魔";
    constexpr static auto ench_item_best_enchantment="最强附魔";

    std::shared_ptr<std::map<std::string,EnchItem>> readItems(std::filesystem::path ench_item_path);

    nlohmann::json toJson(const EnchItem& item);

private:
    /**
     * @brief 把json数据转换成具体的某个附魔种类
     * @param ench_json
     * @return
     */
    static EnchType readEnchType(const nlohmann::json& ench_json);

    /**
     * @brief 把某个物品的json转换成一个具体的EnchItem数据
     * @param name
     * @param ench_item_json
     * @return
     */
    static EnchItem readEnchItem(const std::string& name,const nlohmann::json& ench_item_json);


};

} // GenEnch

#endif //JSONREADER_H
