//
// Created by 91246 on 24-8-29.
//

#ifndef GAMEVERSION_H
#define GAMEVERSION_H

#include <regex>
#include <format>

namespace GenEnch {

struct GameVersion {
    uint32_t data[3]={};

    GameVersion(){}
    GameVersion(uint32_t major,uint32_t minor,uint32_t patch=0):data{major,minor,patch}{}
    GameVersion(const char* version_str):GameVersion(std::string(version_str)) {}
    GameVersion(const std::string& version_str) {
        fromStr(version_str);
    }

    void fromStr(const std::string& version_str) {
        static std::regex version_regex(R"(^(\d+)\.(\d+)(?:\.(\d+))?$)");
        std::smatch match;
        if(std::regex_match(version_str,match,version_regex)) {
            data[0]=stoi(match[1]);
            data[1]=stoi(match[2]);
            if(match.size()>=3 && !match[3].str().empty()) {
                data[2]=stoi(match[3]);
            }
            else {
                data[2]=0;
            }
        }
        else {
            data[0]=0;
            data[1]=0;
            data[2]=0;
        }
    }

    std::string toString() const{
        return std::format("{}.{}.{}",data[0],data[1],data[2]);
    }

    auto operator<=>(const GameVersion &) const = default;
};

} // GenEnch

#endif //GAMEVERSION_H
