#include "Util.h"

size_t WordMapHashFunction::operator()(const std::pair<std::string, std::string>& x) const
{
    std::size_t h1 = std::hash<std::string>{}(x.first);
    std::size_t h2 = std::hash<std::string>{}(x.second);
    return h1 ^ (h2 << 1);
}