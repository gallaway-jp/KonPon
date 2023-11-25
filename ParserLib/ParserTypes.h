#pragma once

#include <vector>
#include <map>
#include <string>
#include <set>
    
using Locations = std::map<size_t, uint8_t>; //value is length of word at location
using WordDict = std::map<std::string, std::map<std::string, Locations>>;
using LocationsDict = std::map<size_t, std::map<std::string, std::set<std::string>>>;