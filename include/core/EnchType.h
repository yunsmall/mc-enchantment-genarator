//
// Created by 91246 on 24-8-28.
//

#ifndef ENCHTYPE_H
#define ENCHTYPE_H

#include <string>
#include <map>

#include "core/GameVersion.h"

namespace GenEnch {

class EnchType {
public:
    std::string name;
    uint32_t max_level=0;
    GameVersion min_version;
    std::map<std::string,std::string> game_priv_names;

    void getEnchName(GameVersion version);
};

} // GenEnch

#endif //ENCHTYPE_H
