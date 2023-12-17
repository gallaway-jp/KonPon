#pragma once

#include <vector>

#define MEMORYMAPLIB_EXPORT

/*!
    \class MemoryMapLib

    \brief The MemoryMapLib class provides methods
    for loading files into memory.
*/
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
