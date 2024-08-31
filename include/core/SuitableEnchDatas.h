//
// Created by 91246 on 24-8-29.
//

#ifndef ENCHKINDDATAS_H
#define ENCHKINDDATAS_H

#include <string>
#include <vector>
#include <map>

namespace GenEnch {

class SuitableEnchDatas {
public:
    std::vector<std::string> suitable_enchantments;
    std::map<std::string,std::vector<std::string>> best_enchantments;
};

} // GenEnch

#endif //ENCHKINDDATAS_H
