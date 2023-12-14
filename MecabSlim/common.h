// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_COMMON_H_
#define MECAB_COMMON_H_

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

#ifdef __CYGWIN__
#define _GLIBCXX_EXPORT_TEMPLATE
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__)
#define NOMINMAX
#define snprintf _snprintf
#endif

#define COPYRIGHT "MeCab: Yet Another Part-of-Speech and Morphological Analyzer\n\
\nCopyright(C) 2001-2012 Taku Kudo \nCopyright(C) 2004-2008 Nippon Telegraph and Telephone Corporation\n"

#define PACKAGE "\"mecab\""
#define VERSION "0.996"
#if defined(_WIN32) && !defined(__CYGWIN__)
#define HAVE_WINDOWS_H 1
#endif //#if defined(_WIN32) && !defined(__CYGWIN__)

#define UNK_DIC_FILE            "unk.dic"
#define DICRC                   "dicrc"
#define BOS_KEY                 "BOS/EOS"

#define DEFAULT_MAX_GROUPING_SIZE 24

#ifndef MECAB_USE_UTF8_ONLY
#define MECAB_USE_UTF8_ONLY
#endif

#ifdef MECAB_USE_UTF8_ONLY
#define MECAB_DEFAULT_CHARSET "UTF-8"
#endif

#ifndef MECAB_DEFAULT_CHARSET
#if defined(_WIN32) && !defined(__CYGWIN__)
#define MECAB_DEFAULT_CHARSET "SHIFT-JIS"
#else
#define MECAB_DEFAULT_CHARSET "EUC-JP"
#endif
#endif

#define NBEST_MAX 512
#define NODE_FREELIST_SIZE 512
#define PATH_FREELIST_SIZE 2048
#define MIN_INPUT_BUFFER_SIZE 8192
#define MAX_INPUT_BUFFER_SIZE (8192*640)
#define BUF_SIZE 8192

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#define WPATH(path) (MeCab::Utf8ToWide(path).c_str())
#else
#define WPATH(path) (path)
#endif

namespace MeCab {
    class die {
    public:
        die() {}
        ~die() {
            
            std::cerr << std::endl;
            exit(-1);
        }
        int operator&(std::ostream&) { return 0; }
    };

    struct whatlog {
        std::ostringstream stream_;
        std::string str_;
        const char* str() {
            str_ = stream_.str();
            return str_.c_str();
        }
    };

    class wlog {
    public:
        wlog(whatlog* what) : what_(what) {
            what_->stream_.clear();
        }
        bool operator&(std::ostream&) {
            return false;
        }
    private:
        whatlog* what_;
    };
}  // MeCab

#define WHAT what_.stream_

#define CHECK_FALSE(condition) \
 if (condition) {} else return \
   wlog(&what_) & what_.stream_ <<              \
      __FILE__ << "(" << __LINE__ << ") [" << #condition << "] "

#define CHECK_DIE(condition) \
(condition) ? 0 : die() & std::cerr << __FILE__ << \
"(" << __LINE__ << ") [" << #condition << "] "

#endif  // MECAB_COMMON_H_