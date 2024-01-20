#pragma once

#include <unordered_map>
#include <string>
typedef std::unordered_map<unsigned char, std::string> SpecialCharsMap;

struct SpecialCharsMapFactory {
    static SpecialCharsMap create();
};
