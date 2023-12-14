// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_MMAP_H
#define MECAB_MMAP_H

#include "MemoryMapLib.h"

#include <errno.h>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//extern "C" {

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#else

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif
//}

#include "common.h"
#include "utils.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace MeCab {

    template <class T> class Mmap {
    private:
        T* text;
        size_t       length;
        std::string  fileName;
        whatlog what_;

        MemoryMapLib memmaplib = MemoryMapLib();
        int handle = -1;

    public:
        T& operator[](size_t n) { return *(text + n); }
        const T& operator[](size_t n) const { return *(text + n); }
        T* begin() { return text; }
        const T* begin()    const { return text; }
        T* end() { return text + size(); }
        const T* end()    const { return text + size(); }
        size_t size() { return length / sizeof(T); }
        const char* what() { return what_.str(); }
        const char* file_name() { return fileName.c_str(); }
        size_t file_size() { return length; }
        bool empty() { return(length == 0); }

        bool open(const char* filename, const char* mode = "r") {
            this->close();
            fileName = std::string(filename);

            handle = memmaplib.open(filename);

            length = memmaplib.size(handle);

            text = reinterpret_cast<T*>(memmaplib.map(handle));
            CHECK_FALSE(text) << "memmaplib.map() failed: " << filename;

            return true;
        }

        void close() {
            if (handle != -1) {
                memmaplib.unmap(handle);
                memmaplib.close(handle);
                handle = -1;
            }
            text = 0;
        }

        Mmap() : text(0), handle(-1) {}

        virtual ~Mmap() { this->close(); }
    };
}
#endif