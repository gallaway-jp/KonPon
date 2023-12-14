#pragma once

#include <vector>
//#include "memorymaplib_global.h"
#define MEMORYMAPLIB_EXPORT
class MEMORYMAPLIB_EXPORT MemoryMapLib
{
public:
    MemoryMapLib();
    ~MemoryMapLib();
    int open(const char* filePath);
    size_t size(int handle);
    void close(int handle);
    char* map(int handle);
    void unmap(int handle);
private:
    std::vector<std::pair<void*, unsigned char*>> fileHandles;
};
