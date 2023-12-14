// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <cstring>
#include <fstream>
#include <iostream>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#endif

#include <stdint.h>

#include "common.h"
#include "mecab.h"
#include "param.h"
#include "utils.h"
#include "conversion.h"

namespace MeCab {

#if defined(_WIN32) && !defined(__CYGWIN__)
    std::wstring Utf8ToWide(const std::string& input) {
        int output_length = ::MultiByteToWideChar(CP_UTF8, 0,
            input.c_str(), -1, NULL, 0);
        output_length = output_length <= 0 ? 0 : output_length - 1;
        if (output_length == 0) {
            return L"";
        }
        scoped_array<wchar_t> input_wide(new wchar_t[output_length + 1]);
        const int result = ::MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1,
            input_wide.get(), output_length + 1);
        std::wstring output;
        if (result > 0) {
            output.assign(input_wide.get());
        }
        return output;
    }

    std::string WideToUtf8(const std::wstring& input) {
        const int output_length = ::WideCharToMultiByte(CP_UTF8, 0,
            input.c_str(), -1, NULL, 0,
            NULL, NULL);
        if (output_length == 0) {
            return "";
        }

        scoped_array<char> input_encoded(new char[output_length + 1]);
        const int result = ::WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1,
            input_encoded.get(),
            output_length + 1, NULL, NULL);
        std::string output;
        if (result > 0) {
            output.assign(input_encoded.get());
        }
        return output;
    }
#endif

    int decode_charset(const char* charset) {
        std::string tmp = charset;
        toLower(&tmp);
        if (tmp == "sjis" || tmp == "shift-jis" ||
            tmp == "shift_jis" || tmp == "cp932")
            return CP932;
        else if (tmp == "euc" || tmp == "euc_jp" ||
            tmp == "euc-jp")
            return EUC_JP;
        else if (tmp == "utf8" || tmp == "utf_8" ||
            tmp == "utf-8")
            return UTF8;
        else if (tmp == "utf16" || tmp == "utf_16" ||
            tmp == "utf-16")
            return UTF16;
        else if (tmp == "utf16be" || tmp == "utf_16be" ||
            tmp == "utf-16be")
            return UTF16BE;
        else if (tmp == "utf16le" || tmp == "utf_16le" ||
            tmp == "utf-16le")
            return UTF16LE;
        else if (tmp == "ascii")
            return ASCII;

        return UTF8;  // default is UTF8
    }

    bool toLower(std::string* s) {
        for (size_t i = 0; i < s->size(); ++i) {
            char c = (*s)[i];
            if ((c >= 'A') && (c <= 'Z')) {
                c += 'a' - 'A';
                (*s)[i] = c;
            }
        }
        return true;
    }

    bool escape_csv_element(std::string* w) {
        if (w->find(',') != std::string::npos ||
            w->find('"') != std::string::npos) {
            std::string tmp = "\"";
            for (size_t j = 0; j < w->size(); j++) {
                if ((*w)[j] == '"') tmp += '"';
                tmp += (*w)[j];
            }
            tmp += '"';
            *w = tmp;
        }
        return true;
    }

    int load_request_type(const Param& param) {
        int request_type = MECAB_ONE_BEST;

        if (param.get<bool>("allocate-sentence")) {
            request_type |= MECAB_ALLOCATE_SENTENCE;
        }

        if (param.get<bool>("partial")) {
            request_type |= MECAB_PARTIAL;
        }

        if (param.get<bool>("all-morphs")) {
            request_type |= MECAB_ALL_MORPHS;
        }

        if (param.get<bool>("marginal")) {
            request_type |= MECAB_MARGINAL_PROB;
        }

        const int nbest = param.get<int>("nbest");
        if (nbest >= 2) {
            request_type |= MECAB_NBEST;
        }

        // DEPRECATED:
        const int lattice_level = param.get<int>("lattice-level");
        if (lattice_level >= 1) {
            request_type |= MECAB_NBEST;
        }

        if (lattice_level >= 2) {
            request_type |= MECAB_MARGINAL_PROB;
        }

        return request_type;
    }

    bool load_dictionary_resource(Param* param) {

        std::string dicdir = param->get<std::string>("dicdir");
        if (dicdir.empty()) {
            dicdir = ".";  // current
        }
        param->set<std::string>("dicdir", dicdir, true);
        param->set<std::string>("cost-factor", "700", false);
        param->set<std::string>("bos-feature", "BOS/EOS,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*", false);
        param->set<std::string>("eval-size", "10", false);
        param->set<std::string>("unk-eval-size", "4", false);
        param->set<std::string>("config-charset", "utf8", false);
        param->set<std::string>("output-format-type", "unidic22", false);
        param->set<std::string>("node-format-unidic22", "%m\\t%f[0],%f[1],%f[2],%f[3],%f[4],%f[5],%f[6],%f[7],%f[8],%f[9],%f[10],%f[11],%f[12],\"%f[13]\",\"%f[14]\",\"%f[15]\",\"%f[16]\",\"%f[17]\",\"%f[18]\",%f[19],%f[20],%f[21],%f[22],%f[23],\"%f[24]\",\"%f[25]\",\"%f[26]\",%f[27],%f[28]\\n", false);
        param->set<std::string>("unk-format-unidic22", "%m\\t%f[0],%f[1],%f[2],%f[3],%f[4],%f[5]\\n", false);
        param->set<std::string>("bos-format-unidic22", "", false);
        param->set<std::string>("eos-format-unidic22", "EOS\\n", false);
        param->set<std::string>("node-format-verbose", "surface:%m\\tpos1:%f[0]\\tpos2:%f[1]\\tpos3:%f[2]\\tpos4:%f[3]\\tcType:%f[4]\\tcForm:%f[5]\\tlForm:%f[6]\\tlemma:%f[7]\\torth:%f[8]\\tpron:%f[9]\\torthBase:%f[10]\\tpronBase:%f[11]\\tgoshu:%f[12]\\tiType:%f[13]\\tiForm:%f[14]\\tfType:%f[15]\\tfForm:%f[16]\\tiConType:%f[17]\\tfConType:%f[18]\\tlType:%f[19]\\tkana:%f[20]\\tkanaBase:%f[21]\\tform:%f[22]\\tformBase:%f[23]\\taType:%f[24]\\taConType:%f[25]\\taModType:%f[26]\\tlid:%f[27]\\tlemma_id:%f[28]\\n", false);
        param->set<std::string>("unk-format-verbose", "surface:%m\\tpos1:%f[0]\\tpos2:%f[1]\\tpos3:%f[2]\\tpos4:%f[3]\\tcType:%f[4]\\tcForm:%f[5]\\n", false);
        param->set<std::string>("bos-format-verbose", "", false);
        param->set<std::string>("eos-format-verbose", "EOS\\n", false);
        param->set<std::string>("node-format-chamame", "\\t%m\\t%f[9]\\t%f[6]\\t%f[7]\\t%F-[0,1,2,3]\\t%f[4]\\t%f[5]\\t%f[23]\\t%f[12]\\n\\n", false);
        param->set<std::string>("unk-format-chamame", "\\t%m\\t\\t\\t%m\\t–¢’mŒê\\t\\t\\t\\t\\n", false);
        param->set<std::string>("bos-format-chamame", "B", false);
        param->set<std::string>("eos-format-chamame", "", false);

        return true;
    }

    namespace {
        // Copied from MurmurHash3.cpp
        // http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp
        //-----------------------------------------------------------------------------
        // Platform-specific functions and macros
        // Microsoft Visual Studio
#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#define ROTL32(x,y)     _rotl(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else   // defined(_MSC_VER)

#define FORCE_INLINE inline __attribute__((always_inline))

        inline uint32_t rotl32(uint32_t x, uint8_t r) {
            return (x << r) | (x >> (32 - r));
        }

#define ROTL32(x,y)     rotl32(x,y)

#endif // !defined(_MSC_VER)

        //-----------------------------------------------------------------------------
        // Block read - if your platform needs to do endian-swapping or can only
        // handle aligned reads, do the conversion here

        FORCE_INLINE uint32_t getblock(const uint32_t* p, int i) {
            return p[i];
        }

        //-----------------------------------------------------------------------------
        // Finalization mix - force all bits of a hash block to avalanche

        FORCE_INLINE uint32_t fmix(uint32_t h) {
            h ^= h >> 16;
            h *= 0x85ebca6b;
            h ^= h >> 13;
            h *= 0xc2b2ae35;
            h ^= h >> 16;

            return h;
        }

        void MurmurHash3_x86_128(const void* key, const int len,
            uint32_t seed, char* out) {
            const uint8_t* data = (const uint8_t*)key;
            const int nblocks = len / 16;

            uint32_t h1 = seed;
            uint32_t h2 = seed;
            uint32_t h3 = seed;
            uint32_t h4 = seed;

            uint32_t c1 = 0x239b961b;
            uint32_t c2 = 0xab0e9789;
            uint32_t c3 = 0x38b34ae5;
            uint32_t c4 = 0xa1e38b93;

            //----------
            // body

            const uint32_t* blocks = (const uint32_t*)(data + nblocks * 16);

            for (int i = -nblocks; i; i++)
            {
                uint32_t k1 = getblock(blocks, i * 4 + 0);
                uint32_t k2 = getblock(blocks, i * 4 + 1);
                uint32_t k3 = getblock(blocks, i * 4 + 2);
                uint32_t k4 = getblock(blocks, i * 4 + 3);

                k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;

                h1 = ROTL32(h1, 19); h1 += h2; h1 = h1 * 5 + 0x561ccd1b;

                k2 *= c2; k2 = ROTL32(k2, 16); k2 *= c3; h2 ^= k2;

                h2 = ROTL32(h2, 17); h2 += h3; h2 = h2 * 5 + 0x0bcaa747;

                k3 *= c3; k3 = ROTL32(k3, 17); k3 *= c4; h3 ^= k3;

                h3 = ROTL32(h3, 15); h3 += h4; h3 = h3 * 5 + 0x96cd1c35;

                k4 *= c4; k4 = ROTL32(k4, 18); k4 *= c1; h4 ^= k4;

                h4 = ROTL32(h4, 13); h4 += h1; h4 = h4 * 5 + 0x32ac3b17;
            }

            //----------
            // tail

            const uint8_t* tail = (const uint8_t*)(data + nblocks * 16);
            uint32_t k1 = 0;
            uint32_t k2 = 0;
            uint32_t k3 = 0;
            uint32_t k4 = 0;

            switch (len & 15)
            {
            case 15: k4 ^= tail[14] << 16;
            case 14: k4 ^= tail[13] << 8;
            case 13: k4 ^= tail[12] << 0;
                k4 *= c4; k4 = ROTL32(k4, 18); k4 *= c1; h4 ^= k4;

            case 12: k3 ^= tail[11] << 24;
            case 11: k3 ^= tail[10] << 16;
            case 10: k3 ^= tail[9] << 8;
            case  9: k3 ^= tail[8] << 0;
                k3 *= c3; k3 = ROTL32(k3, 17); k3 *= c4; h3 ^= k3;

            case  8: k2 ^= tail[7] << 24;
            case  7: k2 ^= tail[6] << 16;
            case  6: k2 ^= tail[5] << 8;
            case  5: k2 ^= tail[4] << 0;
                k2 *= c2; k2 = ROTL32(k2, 16); k2 *= c3; h2 ^= k2;

            case  4: k1 ^= tail[3] << 24;
            case  3: k1 ^= tail[2] << 16;
            case  2: k1 ^= tail[1] << 8;
            case  1: k1 ^= tail[0] << 0;
                k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
            };

            //----------
            // finalization

            h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

            h1 += h2; h1 += h3; h1 += h4;
            h2 += h1; h3 += h1; h4 += h1;

            h1 = fmix(h1);
            h2 = fmix(h2);
            h3 = fmix(h3);
            h4 = fmix(h4);

            h1 += h2; h1 += h3; h1 += h4;
            h2 += h1; h3 += h1; h4 += h1;

            std::memcpy(out, reinterpret_cast<char*>(&h1), 4);
            std::memcpy(out + 4, reinterpret_cast<char*>(&h2), 4);
            std::memcpy(out + 8, reinterpret_cast<char*>(&h3), 4);
            std::memcpy(out + 12, reinterpret_cast<char*>(&h4), 4);
        }
    }

    uint64_t fingerprint(const char* str, size_t size) {
        uint64_t result[2] = { 0 };
        const uint32_t kFingerPrint32Seed = 0xfd14deff;
        MurmurHash3_x86_128(str, Conversion::convertULongLongToInt(size), kFingerPrint32Seed,
            reinterpret_cast<char*>(result));
        return result[0];
    }

    uint64_t fingerprint(const std::string& str) {
        return fingerprint(str.data(), str.size());
    }
}  // namespace MeCab