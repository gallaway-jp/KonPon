//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include "common.h"
#include "feature_index.h"
#include "param.h"
#include "iconv_utils.h"
#include "learner_node.h"
#include "scoped_ptr.h"
#include "string_buffer.h"
#include "utils.h"
#include "conversion.h"

#define BUFSIZE (2048)
#define POSSIZE (64)

#define ADDB(b) do { const int id = this->id((b));  \
    if (id != -1) feature_.push_back(id); } while (0)

#define COPY_FEATURE(ptr) do {                                          \
    feature_.push_back(-1);                                             \
    (ptr) = feature_freelist_.alloc(feature_.size());                   \
    std::copy(feature_.begin(), feature_.end(), const_cast<int *>(ptr)); \
    feature_.clear(); } while (0)

namespace MeCab {

    const char* FeatureIndex::getIndex(char** p, char** column, size_t max) {
        ++(*p);

        bool flg = false;

        if (**p == '?') {
            flg = true;
            ++(*p);
        }  // undef flg

        CHECK_DIE(**p == '[') << "getIndex(): unmatched '['";

        size_t n = 0;
        ++(*p);

        for (;; ++(*p)) {
            switch (**p) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                n = 10 * n + (**p - '0');
                break;
            case ']':
                if (n >= max) {
                    return 0;
                }

                if (flg == true && ((std::strcmp("*", column[n]) == 0)
                    || column[n][0] == '\0')) {
                    return 0;
                }
                return column[n];  // return;
                break;
            default:
                CHECK_DIE(false) << "unmatched '['";
            }
        }

        return 0;
    }

    void FeatureIndex::set_alpha(const double* alpha) {
        alpha_ = alpha;
    }

    bool FeatureIndex::openTemplate(const Param& param) {
        unigram_templs_.clear();
        bigram_templs_.clear();

        const char* unigram_strings[] = {
            "LGOP01:%F[6],%F[7],%F[0],%F[1],%F[2],%F?[3],%F?[8],%F?[10]",
            "LGOP02:%F[6],%F[7],%F[0],%F[1],%F?[2],%F?[8],%F?[10]",
            "LGOP03:%F[6],%F[7],%F[0],%F?[1],%F?[8],%F?[10]",
            "LGOP03:%F[6],%F[7],%F[0],%F?[8],%F?[10]",
            "LGCOP01:%F[6],%F[7],%F[0],%F?[1],%F?[4],%F?[5],%F?[8],%F?[10]",
            "LGCOP02:%F[6],%F[7],%F[0],%F?[4],%F?[5],%F?[8],%F?[10]",
            "LGO01:%F[6],%F[7],%F[0],%F[1],%F[2],%F?[3],%F?[8]",
            "LGO02:%F[6],%F[7],%F[0],%F[1],%F?[2],%F?[8]",
            "LGO03:%F[6],%F[7],%F[0],%F?[1],%F?[8]",
            "LGO03:%F[6],%F[7],%F[0],%F?[8]",
            "LGCO01:%F[6],%F[7],%F[0],%F?[1],%F?[4],%F?[5],%F?[8]",
            "LGCO02:%F[6],%F[7],%F[0],%F?[4],%F?[5],%F?[8]",
            "LG01:%F[6],%F[7],%F[0],%F[1],%F[2],%F?[3]",
            "LG02:%F[6],%F[7],%F[0],%F[1],%F?[2]",
            "LG03:%F[6],%F[7],%F[0],%F?[1]",
            "LG03:%F[6],%F[7],%F[0]",
            "LGC01:%F[6],%F[7],%F[0],%F?[1],%F?[4],%F?[5]",
            "LGC02:%F[6],%F[7],%F[0],%F?[4],%F?[5]",
            "LGCt01:%F[6],%F[7],%F[0],%F?[1],%F?[4]",
            "LGCt02:%F[6],%F[7],%F[0],%F?[4]",
            "L01:%F[6],%F[7]",
            "LGP01:%F[6],%F[7],%F[0],%F[1],%F[2],%F?[3],%F?[10]",
            "LGP02:%F[6],%F[7],%F[0],%F[1],%F?[2],%F?[10]",
            "LGP03:%F[6],%F[7],%F[0],%F?[1],%F?[10]",
            "LGP03:%F[6],%F[7],%F[0],%F?[10]",
            "LGCP01:%F[6],%F[7],%F[0],%F?[1],%F?[4],%F?[5],%F?[10]",
            "LGCP02:%F[6],%F[7],%F[0],%F?[4],%F?[5],%F?[10]",
            "GC01:%F[0],%F?[4]",
            "GC02:%F[0],%F?[1],%F?[4]",
            "GC03:%F[0],%F?[4],%F?[5]",
            "GC04:%F[0],%F?[1],%F?[4],%F?[5]",
            "G01:%F[0]",
            "G02:%F[0],%F?[1]",
            "G03:%F[0],%F[1],%F?[2]",
            "G04:%F[0],%F[1],%F[2],%F?[3]",
            "C01:%F?[4]",
            "C03:%F?[4],%F?[5]",
            "O01:%F?[8]",
            "O02:%F?[9]",
            "P01:%F?[10]",
            "P02:%F?[11]",
            "W01:%F?[12]",
            "WG01:%F?[12],%F[0]",
            "T01:%t",
            "TG01:%t,%F[0]",
            "GOP01:%F[0],%F[1],%F[2],%F?[3],%F?[8],%F?[10]",
            "GOP02:%F[0],%F[1],%F?[2],%F?[8],%F?[10]",
            "GOP03:%F[0],%F?[1],%F?[8],%F?[10]",
            "GOP03:%F[0],%F?[8],%F?[10]",
            "GCOP01:%F[0],%F?[1],%F?[4],%F?[5],%F?[8],%F?[10]",
            "GCOP02:%F[0],%F?[4],%F?[5],%F?[8],%F?[10]",
            "GO01:%F[0],%F[1],%F[2],%F?[3],%F?[8]",
            "GO02:%F[0],%F[1],%F?[2],%F?[8]",
            "GO03:%F[0],%F?[1],%F?[8]",
            "GO03:%F[0],%F?[8]",
            "GCO01:%F[0],%F?[1],%F?[4],%F?[5],%F?[8]",
            "GCO02:%F[0],%F?[4],%F?[5],%F?[8]",
            "GP01:%F[0],%F[1],%F[2],%F?[3],%F?[10]",
            "GP02:%F[0],%F[1],%F?[2],%F?[10]",
            "GP03:%F[0],%F?[1],%F?[10]",
            "GP03:%F[0],%F?[10]",
            "GCP01:%F[0],%F?[1],%F?[4],%F?[5],%F?[10]",
            "GCP02:%F[0],%F?[4],%F?[5],%F?[10]",
            "LGObPb01:%F[6],%F[7],%F[0],%F[1],%F[2],%F?[3],%F?[9],%F?[11]",
            "LGObPb02:%F[6],%F[7],%F[0],%F[1],%F?[2],%F?[9],%F?[11]",
            "LGObPb03:%F[6],%F[7],%F[0],%F?[1],%F?[9],%F?[11]",
            "LGObPb03:%F[6],%F[7],%F[0],%F?[9],%F?[11]",
            "LGCObPb01:%F[6],%F[7],%F[0],%F?[1],%F?[4],%F?[9],%F?[11]",
            "LGCObPb02:%F[6],%F[7],%F[0],%F?[4],%F?[9],%F?[11]",
            "LGOb01:%F[6],%F[7],%F[0],%F[1],%F[2],%F?[3],%F?[9]",
            "LGOb02:%F[6],%F[7],%F[0],%F[1],%F?[2],%F?[9]",
            "LGOb03:%F[6],%F[7],%F[0],%F?[1],%F?[9]",
            "LGOb03:%F[6],%F[7],%F[0],%F?[9]",
            "LGCOb01:%F[6],%F[7],%F[0],%F?[1],%F?[4],%F?[9]",
            "LGCOb02:%F[6],%F[7],%F[0],%F?[4],%F?[9]",
            "GObPb01:%F[0],%F[1],%F[2],%F?[3],%F?[9],%F?[11]",
            "GObPb02:%F[0],%F[1],%F?[2],%F?[9],%F?[11]",
            "GObPb03:%F[0],%F?[1],%F?[9],%F?[11]",
            "GObPb03:%F[0],%F?[9],%F?[11]",
            "GCObPb01:%F[0],%F?[1],%F?[4],%F?[9],%F?[11]",
            "GCObPb02:%F[0],%F?[4],%F?[9],%F?[11]",
            "GOb01:%F[0],%F[1],%F[2],%F?[3],%F?[9]",
            "GOb02:%F[0],%F[1],%F?[2],%F?[9]",
            "GOb03:%F[0],%F?[1],%F?[9]",
            "GOb03:%F[0],%F?[9]",
            "GCOb01:%F[0],%F?[1],%F?[4],%F?[9]",
            "GCOb02:%F[0],%F?[4],%F?[9]",
            "GPb01:%F[0],%F[1],%F[2],%F?[3],%F?[11]",
            "GPb02:%F[0],%F[1],%F?[2],%F?[11]",
            "GPb03:%F[0],%F?[1],%F?[11]",
            "GPb03:%F[0],%F?[11]",
            "GCPb01:%F[0],%F?[1],%F?[4],%F?[11]",
            "GCPb02:%F[0],%F?[4],%F?[11]",
            "CA01:%F?[5],%F?[15]",
        };

        const char* bigram_strings[] = {
            "G_G01:%L[0]/%R[0]",
            "G_G02:%L[0]/%R[0],%R?[1]",
            "G_G03:%L[0]/%R[0],%R[1],%R?[2]",
            "G_G04:%L[0]/%R[0],%R[1],%R[2],%R?[3]",
            "G_G05:%L[0],%L?[1]/%R[0]",
            "G_G06:%L[0],%L?[1]/%R[0],%R?[1]",
            "G_G07:%L[0],%L?[1]/%R[0],%R[1],%R?[2]",
            "G_G08:%L[0],%L?[1]/%R[0],%R[1],%R[2],%R?[3]",
            "G_G09:%L[0],%L[1],%L?[2]/%R[0]",
            "G_G10:%L[0],%L[1],%L?[2]/%R[0],%R?[1]",
            "G_G11:%L[0],%L[1],%L?[2]/%R[0],%R[1],%R?[2]",
            "G_G12:%L[0],%L[1],%L?[2]/%R[0],%R[1],%R[2],%R?[3]",
            "G_G13:%L[0],%L[1],%L[2],%L?[3]/%R[0]",
            "G_G14:%L[0],%L[1],%L[2],%L?[3]/%R[0],%R?[1]",
            "G_G15:%L[0],%L[1],%L[2],%L?[3]/%R[0],%R[1],%R?[2]",
            "G_G16:%L[0],%L[1],%L[2],%L?[3]/%R[0],%R[1],%R[2],%R?[3]",
            "G_O01:%L[0]/%R?[7]",
            "G_O02:%L[0],%L?[1]/%R?[7]",
            "G_O03:%L[0],%L[1],%L?[2]/%R?[7]",
            "G_O04:%L[0],%L[1],%L[2],%L?[3]/%R?[7]",
            "G_O05:%L[0]/%R?[6]",
            "G_O06:%L[0],%L?[1]/%R?[6]",
            "G_O07:%L[0],%L[1],%L?[2]/%R?[6]",
            "G_O08:%L[0],%L[1],%L[2],%L?[3]/%R?[6]",
            "G_C01:%L[0]/%R[0],%R?[4]",
            "G_C02:%L[0],%L?[1]/%R[0],%R?[4]",
            "G_C03:%L[0]/%R[0],%R?[4],%R[5]",
            "G_C04:%L[0],%L?[1]/%R[0],%R?[4],%R[5]",
            "O_G01:%L?[7]/%R[0]",
            "O_G02:%L?[7]/%R[0],%R?[1]",
            "O_G03:%L?[7]/%R[0],%R[1],%R?[2]",
            "O_G04:%L?[7]/%R[0],%R[1],%R[2],%R?[3]",
            "O_G05:%L?[6]/%R[0]",
            "O_G06:%L?[6]/%R[0],%R?[1]",
            "O_G07:%L?[6]/%R[0],%R[1],%R?[2]",
            "O_G08:%L?[6]/%R[0],%R[1],%R[2],%R?[3]",
            "O_O01:%L?[6]/%R?[6]",
            "O_O02:%L?[6]/%R?[7]",
            "O_O03:%L?[7]/%R?[7]",
            "O_C01:%L?[6]/%R[0],%R?[4]",
            "O_C02:%L?[6]/%R[0],%R?[4],%R?[5]",
            "O_C03:%L?[7]/%R[0],%R?[4]",
            "O_C04:%L?[7]/%R[0],%R?[4],%R?[5]",
            "C_G01:%L[0],%L?[4]/%R[0]",
            "C_G02:%L[0],%L?[4]/%R[0],%R?[1]",
            "C_G03:%L[0],%L?[4]/%R[0],%R[1],%R?[2]",
            "C_G04:%L[0],%L?[4]/%R[0],%R[1],%R[2],%R?[3]",
            "C_G05:%L[0],%L?[4],%L?[5]/%R[0]",
            "C_G06:%L[0],%L?[4],%L?[5]/%R[0],%R?[1]",
            "C_G07:%L[0],%L?[4],%L?[5]/%R[0],%R[1],%R?[2]",
            "C_G08:%L[0],%L?[4],%L?[5]/%R[0],%R[1],%R[2],%R?[3]",
            "C_O01:%L[0],%L?[4]/%R?[6]",
            "C_O02:%L[0],%L?[4]/%R?[7]",
            "C_O03:%L[0],%L?[4],%L?[5]/%R?[6]",
            "C_O04:%L[0],%L?[4],%L?[5]/%R?[7]",
            "C_C01:%L[0],%L?[4]/%R[0],%R?[4]",
            "C_C02:%L[0],%L?[4],%L?[5]/%R[0],%R?[4]",
            "C_C03:%L[0],%L?[4]/%R[0],%R?[4]%R?[5]",
            "C_C04:%L[0],%L?[4],%L?[5]/%R[0],%R?[4]%R?[5]",
            "Cf_G01:%L?[5]/%R[0]",
            "Cf_G02:%L?[5]/%R[0],%R?[1]",
            "Cf_G03:%L?[5]/%R[0],%R[1],%R?[2]",
            "Cf_G04:%L?[5]/%R[0],%R[1],%R[2],%R?[3]",
            "Cf_G01:%L[0],%L?[5]/%R[0]",
            "Cf_G02:%L[0],%L?[5]/%R[0],%R?[1]",
            "Cf_G03:%L[0],%L?[5]/%R[0],%R[1],%R?[2]",
            "Cf_G04:%L[0],%L?[5]/%R[0],%R[1],%R[2],%R?[3]",
            "I_I %L?[11]/%R?[9],%R?[10]",
            "F_F %L?[9],%L?[10]/%R?[11]",
            "W_W01:%L?[8]/%R?[8]",
            "C_C01:%L?[4]/%R?[4]",
            "W_C01:%L?[8]/%R?[4]",
            "C_W01:%L?[4]/%R?[8]",
            "W_O01:%L?[8]/%R?[6]",
            "W_O02:%L?[8]/%R?[7]",
            "O_W01:%L?[6]/%R?[8]",
            "O_W02:%L?[7]/%R?[8]",
            "GC_GC01:%L[0],%L?[1],%L?[4],%L?[5]/%R[0],%R?[1],%R?[4],%R?[5]",
            "GC_GC02:%L[0],%L?[4],%L?[5]/%R[0],%R?[1],%R?[4],%R?[5]",
            "GC_GC03:%L[0],%L?[1],%L?[4],%L?[5]/%R[0],%R?[4],%R?[5]",
            "GC_GC04:%L[0],%L?[4],%L?[5]/%R[0],%R?[4],%R?[5]",
            "A_A01:%L?[12]/%R?[12]",
            "A_A02:%L?[5],%L?[14]/%R?[12]",
            "A_A03:%L?[12]/%R?[5],%R?[14]",
            "A_A04:%L?[5],%L?[14]/%R?[5],%R?[14]",
            "G_Ac01:%L[0]/%R?[13]",
            "G_Ac02:%L[0],%L?[1]/%R?[13]",
            "G_Ac03:%L[0],%L?[1],%L?[2]/%R?[13]",
            "Ac_G01:%L?[13]/%R[0]",
            "Ac_G02:%L?[13]/%R[0],%R?[1]",
            "Ac_G03:%L?[13]/%R[0],%R?[1],%R?[2]"
        };

        for (auto unigram_string : unigram_strings) {
            unigram_templs_.push_back(this->strdup(unigram_string));
        }
        for (auto bigram_string : bigram_strings) {
            bigram_templs_.push_back(this->strdup(bigram_string));
        }

        // second, open rewrite rules
        rewrite_.open();

        return true;
    }

    bool EncoderFeatureIndex::open(const Param& param) {
        return openTemplate(param);
    }

    bool DecoderFeatureIndex::open(const Param& param) {
        const std::string modelfile = param.get<std::string>("model");
        // open the file as binary mode again and fallback to text file
        if (!openBinaryModel(param)) {
            std::cout << modelfile
                << " is not a binary model. reopen it as text mode..."
                << std::endl;
            CHECK_DIE(openTextModel(param)) <<
                "no such file or directory: " << modelfile;
        }

        if (!openTemplate(param)) {
            close();
            return false;
        }

        return true;
    }


    bool DecoderFeatureIndex::openFromArray(const char* begin, const char* end) {
        const char* ptr = begin;
        unsigned int maxid = 0;
        read_static<unsigned int>(&ptr, maxid);
        maxid_ = static_cast<size_t>(maxid);
        const size_t file_size = static_cast<size_t>(end - begin);
        const size_t expected_file_size =
            (sizeof(double) + sizeof(uint64_t)) * maxid_ + sizeof(maxid) + 32;
        if (expected_file_size != file_size) {
            return false;
        }
        charset_ = ptr;
        ptr += 32;
        alpha_ = reinterpret_cast<const double*>(ptr);
        ptr += (sizeof(alpha_[0]) * maxid_);
        key_ = reinterpret_cast<const uint64_t*>(ptr);
        return true;
    }

    bool DecoderFeatureIndex::openBinaryModel(const Param& param) {
        const std::string modelfile = param.get<std::string>("model");
        CHECK_DIE(mmap_.open(modelfile.c_str())) << mmap_.what();
        if (!openFromArray(mmap_.begin(), mmap_.end())) {
            mmap_.close();
            return false;
        }
        const std::string to = param.get<std::string>("charset");
        CHECK_DIE(decode_charset(charset_) ==
            decode_charset(to.c_str()))
            << "model charset and dictionary charset are different. "
            << "model_charset=" << charset_
            << " dictionary_charset=" << to;

        return true;
    }

    bool DecoderFeatureIndex::openTextModel(const Param& param) {
        const std::string modelfile = param.get<std::string>("model");
        CHECK_DIE(FeatureIndex::convert(param, modelfile.c_str(), &model_buffer_));
        return openFromArray(model_buffer_.data(),
            model_buffer_.data() + model_buffer_.size());
    }

    void DecoderFeatureIndex::clear() {
        feature_freelist_.free();
    }

    void EncoderFeatureIndex::clear() {}

    void EncoderFeatureIndex::clearcache() {
        feature_cache_.clear();
        rewrite_.clear();
    }

    void EncoderFeatureIndex::close() {
        dic_.clear();
        feature_cache_.clear();
        maxid_ = 0;
    }

    void DecoderFeatureIndex::close() {
        mmap_.close();
        model_buffer_.clear();
        maxid_ = 0;
    }

    void FeatureIndex::calcCost(LearnerNode* node) {
        node->wcost = 0.0;
        if (node->stat == MECAB_EOS_NODE) return;
        for (const int* f = node->fvector; *f != -1; ++f) {
            node->wcost += alpha_[*f];
        }
    }

    void FeatureIndex::calcCost(LearnerPath* path) {
        if (is_empty(path)) return;
        path->cost = path->rnode->wcost;
        for (const int* f = path->fvector; *f != -1; ++f) {
            path->cost += alpha_[*f];
        }
    }

    const char* FeatureIndex::strdup(const char* p) {
        size_t len = std::strlen(p);
        size_t newLen = len + 1;
        char* q = char_freelist_.alloc(newLen);
        strncpy_s(q, newLen, p, newLen);
        return q;
    }

    bool DecoderFeatureIndex::buildFeature(LearnerPath* path) {
        path->rnode->wcost = path->cost = 0.0;

        std::string ufeature1;
        std::string lfeature1;
        std::string rfeature1;
        std::string ufeature2;
        std::string lfeature2;
        std::string rfeature2;

        CHECK_DIE(rewrite_.rewrite2(path->lnode->feature,
            &ufeature1,
            &lfeature1,
            &rfeature1))
            << " cannot rewrite pattern: "
            << path->lnode->feature;

        CHECK_DIE(rewrite_.rewrite2(path->rnode->feature,
            &ufeature2,
            &lfeature2,
            &rfeature2))
            << " cannot rewrite pattern: "
            << path->rnode->feature;

        if (!buildUnigramFeature(path, ufeature2.c_str())) {
            return false;
        }

        if (!buildBigramFeature(path, rfeature1.c_str(), lfeature2.c_str())) {
            return false;
        }

        return true;
    }

    bool EncoderFeatureIndex::buildFeature(LearnerPath* path) {
        path->rnode->wcost = path->cost = 0.0;

        std::string ufeature1;
        std::string lfeature1;
        std::string rfeature1;
        std::string ufeature2;
        std::string lfeature2;
        std::string rfeature2;

        CHECK_DIE(rewrite_.rewrite2(path->lnode->feature,
            &ufeature1,
            &lfeature1,
            &rfeature1))
            << " cannot rewrite pattern: "
            << path->lnode->feature;

        CHECK_DIE(rewrite_.rewrite2(path->rnode->feature,
            &ufeature2,
            &lfeature2,
            &rfeature2))
            << " cannot rewrite pattern: "
            << path->rnode->feature;

        {
            os_.clear();
            os_ << ufeature2 << ' ' << path->rnode->char_type << '\0';
            const std::string key(os_.str());
            std::map<std::string, std::pair<const int*, size_t> >::iterator
                it = feature_cache_.find(key);
            if (it != feature_cache_.end()) {
                path->rnode->fvector = it->second.first;
                it->second.second++;
            }
            else {
                if (!buildUnigramFeature(path, ufeature2.c_str())) {
                    return false;
                }
                feature_cache_.insert(std::pair
                    <std::string, std::pair<const int*, size_t> >
                    (key,
                        std::pair<const int*, size_t>
                        (path->rnode->fvector, 1)));
            }
        }

        {
            os_.clear();
            os_ << rfeature1 << ' ' << lfeature2 << '\0';
            std::string key(os_.str());
            std::map<std::string, std::pair<const int*, size_t> >::iterator
                it = feature_cache_.find(key);
            if (it != feature_cache_.end()) {
                path->fvector = it->second.first;
                it->second.second++;
            }
            else {
                if (!buildBigramFeature(path, rfeature1.c_str(), lfeature2.c_str()))
                    return false;
                feature_cache_.insert(std::pair
                    <std::string, std::pair<const int*, size_t> >
                    (key, std::pair<const int*, size_t>
                        (path->fvector, 1)));
            }
        }

        CHECK_DIE(path->fvector) << " fvector is NULL";
        CHECK_DIE(path->rnode->fvector) << "fevector is NULL";

        return true;
    }

    bool FeatureIndex::buildUnigramFeature(LearnerPath* path,
        const char* ufeature) {
        scoped_fixed_array<char, BUFSIZE> ubuf;
        scoped_fixed_array<char*, POSSIZE> F;

        feature_.clear();
        strncpy_s(ubuf.get(), ubuf.size(), ufeature, ubuf.size());
        const size_t usize = tokenizeCSV(ubuf.get(), F.get(), F.size());

        for (std::vector<const char*>::const_iterator it = unigram_templs_.begin();
            it != unigram_templs_.end(); ++it) {
            const char* p = *it;
            os_.clear();

            for (; *p; p++) {
                switch (*p) {
                default: os_ << *p; break;
                case '\\': os_ << getEscapedChar(*++p); break;
                case '%': {
                    switch (*++p) {
                    case 'F': {
                        const char* r = getIndex(const_cast<char**>(&p), F.get(), usize);
                        if (!r) goto NEXT;
                        os_ << r;
                    } break;
                    case 't':  os_ << (size_t)path->rnode->char_type;     break;
                    case 'u':  os_ << ufeature; break;
                    case 'w':
                        if (path->rnode->stat == MECAB_NOR_NODE) {
                            os_.write(path->rnode->surface, path->rnode->length);
                        }
                    default:
                        CHECK_DIE(false) << "unknown meta char: " << *p;
                    }
                }
                }
            }

            os_ << '\0';
            ADDB(os_.str());

        NEXT: continue;
        }

        COPY_FEATURE(path->rnode->fvector);

        return true;
    }

    bool FeatureIndex::buildBigramFeature(LearnerPath* path,
        const char* rfeature,
        const char* lfeature) {
        scoped_fixed_array<char, BUFSIZE> rbuf;
        scoped_fixed_array<char, BUFSIZE> lbuf;
        scoped_fixed_array<char*, POSSIZE> R;
        scoped_fixed_array<char*, POSSIZE> L;

        feature_.clear();
        strncpy_s(lbuf.get(), lbuf.size(), rfeature, lbuf.size());
        strncpy_s(rbuf.get(), rbuf.size(), lfeature, rbuf.size());

        const size_t lsize = tokenizeCSV(lbuf.get(), L.get(), L.size());
        const size_t rsize = tokenizeCSV(rbuf.get(), R.get(), R.size());

        for (std::vector<const char*>::const_iterator it = bigram_templs_.begin();
            it != bigram_templs_.end(); ++it) {
            const char* p = *it;
            os_.clear();

            for (; *p; p++) {
                switch (*p) {
                default: os_ << *p; break;
                case '\\': os_ << getEscapedChar(*++p); break;
                case '%': {
                    switch (*++p) {
                    case 'L': {
                        const char* r = getIndex(const_cast<char**>(&p), L.get(), lsize);
                        if (!r) goto NEXT;
                        os_ << r;
                    } break;
                    case 'R': {
                        const char* r = getIndex(const_cast<char**>(&p), R.get(), rsize);
                        if (!r) goto NEXT;
                        os_ << r;
                    } break;
                    case 'l':  os_ << lfeature; break;  // use lfeature as it is
                    case 'r':  os_ << rfeature; break;
                    default:
                        CHECK_DIE(false) << "unknown meta char: " << *p;
                    }
                }
                }
            }

            os_ << '\0';

            ADDB(os_.str());

        NEXT: continue;
        }

        COPY_FEATURE(path->fvector);

        return true;
    }

    int DecoderFeatureIndex::id(const char* key) {
        const uint64_t fp = fingerprint(key, std::strlen(key));
        const uint64_t* result = std::lower_bound(key_,
            key_ + maxid_,
            fp);
        if (result == key_ + maxid_ || *result != fp) {
            return -1;
        }
        const int n = static_cast<int>(result - key_);
        CHECK_DIE(key_[n] == fp);
        return n;
    }

    int EncoderFeatureIndex::id(const char* key) {
        std::map<std::string, int>::const_iterator it = dic_.find(key);
        if (it == dic_.end()) {
            dic_.insert(std::pair<std::string, int>(std::string(key), maxid_));
            return Conversion::convertULongLongToInt(maxid_++);
        }
        else {
            return it->second;
        }
        return -1;
    }

    void EncoderFeatureIndex::shrink(size_t freq,
        std::vector<double>* observed) {
        std::vector<size_t> freqv;
        // count fvector
        freqv.resize(maxid_);
        for (std::map<std::string, std::pair<const int*, size_t> >::const_iterator
            it = feature_cache_.begin();
            it != feature_cache_.end(); ++it) {
            for (const int* f = it->second.first; *f != -1; ++f) {
                freqv[*f] += it->second.second;  // freq
            }
        }

        if (freq <= 1) {
            return;
        }

        // make old2new map
        maxid_ = 0;
        std::map<int, int> old2new;
        for (size_t i = 0; i < freqv.size(); ++i) {
            if (freqv[i] >= freq) {
                old2new.insert(std::pair<int, int>(i, maxid_++));
            }
        }

        // update dic_
        for (std::map<std::string, int>::iterator
            it = dic_.begin(); it != dic_.end();) {
            std::map<int, int>::const_iterator it2 = old2new.find(it->second);
            if (it2 != old2new.end()) {
                it->second = it2->second;
                ++it;
            }
            else {
                dic_.erase(it++);
            }
        }

        // update all fvector
        for (std::map<std::string, std::pair<const int*, size_t> >::const_iterator
            it = feature_cache_.begin(); it != feature_cache_.end(); ++it) {
            int* to = const_cast<int*>(it->second.first);
            for (const int* f = it->second.first; *f != -1; ++f) {
                std::map<int, int>::const_iterator it2 = old2new.find(*f);
                if (it2 != old2new.end()) {
                    *to = it2->second;
                    ++to;
                }
            }
            *to = -1;
        }

        // update observed vector
        std::vector<double> observed_new(maxid_);
        for (size_t i = 0; i < observed->size(); ++i) {
            std::map<int, int>::const_iterator it = old2new.find(static_cast<int>(i));
            if (it != old2new.end()) {
                observed_new[it->second] = (*observed)[i];
            }
        }

        // copy
        *observed = observed_new;

        return;
    }

    bool FeatureIndex::convert(const Param& param,
        const char* txtfile, std::string* output) {
        std::ifstream ifs(WPATH(txtfile));
        CHECK_DIE(ifs) << "no such file or directory: " << txtfile;
        scoped_fixed_array<char, BUF_SIZE> buf;
        char* column[4];
        std::vector<std::pair<uint64_t, double> > dic;
        std::string model_charset;

        while (ifs.getline(buf.get(), buf.size())) {
            if (std::strlen(buf.get()) == 0) {
                break;
            }
            CHECK_DIE(tokenize2(buf.get(), ":", column, 2) == 2)
                << "format error: " << buf.get();
            if (std::string(column[0]) == "charset") {
                model_charset = column[1] + 1;
            }
        }

        std::string from = param.get<std::string>("dictionary-charset");
        std::string to = param.get<std::string>("charset");

        if (!from.empty()) {
            CHECK_DIE(decode_charset(model_charset.c_str())
                == decode_charset(from.c_str()))
                << "dictionary charset and model charset are different. "
                << "dictionary_charset=" << from
                << " model_charset=" << model_charset;
        }
        else {
            from = model_charset;
        }

        if (to.empty()) {
            to = from;
        }

        Iconv iconv;
        CHECK_DIE(iconv.open(from.c_str(), to.c_str()))
            << "cannot create model from=" << from
            << " to=" << to;

        while (ifs.getline(buf.get(), buf.size())) {
            CHECK_DIE(tokenize2(buf.get(), "\t", column, 2) == 2)
                << "format error: " << buf.get();
            std::string feature = column[1];
            CHECK_DIE(iconv.convert(&feature));
            const uint64_t fp = fingerprint(feature);
            const double alpha = atof(column[0]);
            dic.push_back(std::pair<uint64_t, double>(fp, alpha));
        }

        output->clear();
        unsigned int size = static_cast<unsigned int>(dic.size());
        output->append(reinterpret_cast<const char*>(&size), sizeof(size));

        char charset_buf[32];
        std::fill(charset_buf, charset_buf + sizeof(charset_buf), '\0');
        strncpy_s(charset_buf, sizeof(charset_buf), to.c_str(), sizeof(charset_buf)-1);
        output->append(reinterpret_cast<const char*>(charset_buf),
            sizeof(charset_buf));

        std::sort(dic.begin(), dic.end());

        for (size_t i = 0; i < dic.size(); ++i) {
            const double alpha = dic[i].second;
            output->append(reinterpret_cast<const char*>(&alpha), sizeof(alpha));
        }

        for (size_t i = 0; i < dic.size(); ++i) {
            const uint64_t fp = dic[i].first;
            output->append(reinterpret_cast<const char*>(&fp), sizeof(fp));
        }

        return true;
    }

    bool EncoderFeatureIndex::reopen(const char* filename,
        const char* dic_charset,
        std::vector<double>* alpha,
        Param* param) {
        close();
        std::ifstream ifs(WPATH(filename));
        if (!ifs) {
            return false;
        }

        scoped_fixed_array<char, BUF_SIZE> buf;
        char* column[8];

        std::string model_charset;

        while (ifs.getline(buf.get(), buf.size())) {
            if (std::strlen(buf.get()) == 0) {
                break;
            }
            CHECK_DIE(tokenize2(buf.get(), ":", column, 2) == 2)
                << "format error: " << buf.get();
            if (std::string(column[0]) == "charset") {
                model_charset = column[1] + 1;
            }
            else {
                param->set<std::string>(column[0], column[1] + 1, true);
            }
        }

        CHECK_DIE(dic_charset);
        CHECK_DIE(!model_charset.empty()) << "charset is empty";

        Iconv iconv;
        CHECK_DIE(iconv.open(model_charset.c_str(), dic_charset))
            << "cannot create model from=" << model_charset
            << " to=" << dic_charset;

        alpha->clear();
        CHECK_DIE(maxid_ == 0);
        CHECK_DIE(dic_.empty());

        while (ifs.getline(buf.get(), buf.size())) {
            CHECK_DIE(tokenize2(buf.get(), "\t", column, 2) == 2)
                << "format error: " << buf.get();
            std::string feature = column[1];
            CHECK_DIE(iconv.convert(&feature));
            dic_.insert(std::make_pair(feature, maxid_++));
            alpha->push_back(atof(column[0]));
        }

        return true;
    }

    bool EncoderFeatureIndex::save(const char* filename, const char* header) const {
        CHECK_DIE(header);
        CHECK_DIE(alpha_);

        std::ofstream ofs(WPATH(filename));
        if (!ofs) {
            return false;
        }

        ofs.setf(std::ios::fixed, std::ios::floatfield);
        ofs.precision(16);

        ofs << header;
        ofs << std::endl;

        for (std::map<std::string, int>::const_iterator it = dic_.begin();
            it != dic_.end(); ++it) {
            ofs << alpha_[it->second] << '\t' << it->first << '\n';
        }

        return true;
    }
}