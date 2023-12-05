#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <unordered_set>
    
using Locations = std::unordered_map<size_t, uint8_t>; //value is length of word at location
using WordDict = std::unordered_map<std::string, std::unordered_map<std::string, Locations>>;
using LocationsDict = std::unordered_map<size_t, std::unordered_map<std::string, std::unordered_set<std::string>>>;