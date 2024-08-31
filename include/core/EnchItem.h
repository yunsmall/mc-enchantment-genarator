//
// Created by 91246 on 24-8-29.
//

#ifndef ENCHITEM_H
#define ENCHITEM_H

#include <string>
#include <map>
#include <vector>

#include "core/SuitableEnchDatas.h"

namespace GenEnch {

class EnchItem {
public:
    explicit EnchItem(const std::string& name);

    ~EnchItem() = default;

    //子名字：游戏内名字
    std::map<std::string,std::string> game_names;
    //版本：数据
    std::map<std::string,SuitableEnchDatas> enchDatas;
    std::string name;
};

} // EnchGen

#endif //ENCHITEM_H
