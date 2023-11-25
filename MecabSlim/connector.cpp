//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <fstream>
#include <sstream>
#include "common.h"
#include "connector.h"
#include "mmap.h"
#include "param.h"
#include "utils.h"
#include "conversion.h"

namespace MeCab {

    bool Connector::open(const Param& param) {
        const std::string filename = create_filename
        (param.get<std::string>("dicdir"), MATRIX_FILE);
        return open(filename.c_str());
    }

    bool Connector::open(const char* filename,
        const char* mode) {
        CHECK_FALSE(cmmap_->open(filename, mode))
            << "cannot open: " << filename;

        matrix_ = cmmap_->begin();

        CHECK_FALSE(matrix_) << "matrix is NULL";
        CHECK_FALSE(cmmap_->size() >= 2)
            << "file size is invalid: " << filename;

        lsize_ = static_cast<unsigned short>((*cmmap_)[0]);
        rsize_ = static_cast<unsigned short>((*cmmap_)[1]);

        CHECK_FALSE(static_cast<size_t>(lsize_ * rsize_ + 2)
            == cmmap_->size())
            << "file size is invalid: " << filename;

        matrix_ = cmmap_->begin() + 2;
        return true;
    }

    void Connector::close() {
        cmmap_->close();
    }

    void Connector::set_left_size(size_t lsize)
    {
        lsize_ = Conversion::convertULongLongToUShort(lsize);
    }
    void Connector::set_right_size(size_t rsize)
    {
        rsize_ = Conversion::convertULongLongToUShort(rsize);
    }

    bool Connector::openText(const char* filename) {
        std::ifstream ifs(WPATH(filename));
        if (!ifs) {
            WHAT << "no such file or directory: " << filename;
            return false;
        }
        char* column[2];
        scoped_fixed_array<char, BUF_SIZE> buf;
        ifs.getline(buf.get(), buf.size());
        CHECK_DIE(tokenize2(buf.get(), "\t ", column, 2) == 2)
            << "format error: " << buf.get();
        lsize_ = std::atoi(column[0]);
        rsize_ = std::atoi(column[1]);
        return true;
    }
}