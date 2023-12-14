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
        const std::string filename = ":/dictionary/unidic/matrix.bin";
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
}