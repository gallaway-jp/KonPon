#pragma once

#include <string>

// Hash function  
struct WordMapHashFunction
{
    size_t operator()(const std::pair<std::string, std::string>& x) const;
};