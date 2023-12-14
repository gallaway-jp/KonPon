//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include "common.h"
#include "dictionary_rewriter.h"
#include "iconv_utils.h"
#include "scoped_ptr.h"
#include "utils.h"

namespace {

    using namespace MeCab;

    void append_rewrite_rule(RewriteRules* r, char* str) {
        char* col[3];
        const size_t n = tokenize2(str, " \t", col, 3);
        CHECK_DIE(n >= 2) << "format error: " << str;
        r->resize(r->size() + 1);
        std::string tmp;
        if (n >= 3) {
            tmp = col[1];
            tmp += ' ';
            tmp += col[2];
            col[1] = const_cast<char*>(tmp.c_str());
        }
        r->back().set_pattern(col[0], col[1]);
    }

    bool match_rewrite_pattern(const char* pat,
        const char* str) {
        if (pat[0] == '*' || std::strcmp(pat, str) == 0)
            return true;

        size_t len = std::strlen(pat);
        if (len >= 3 && pat[0] == '(' && pat[len - 1] == ')') {
            scoped_fixed_array<char, BUF_SIZE> buf;
            scoped_fixed_array<char*, BUF_SIZE> col;
            CHECK_DIE(len < buf.size() - 3) << "too long parameter";
            strncpy_s(buf.get(), buf.size(), pat + 1, buf.size());
            buf[len - 2] = '\0';
            const size_t n = tokenize(buf.get(), "|", col.get(), col.size());
            CHECK_DIE(n < col.size()) << "too long OR nodes";
            for (size_t i = 0; i < n; ++i) {
                if (std::strcmp(str, col[i]) == 0) return true;
            }
        }
        return false;
    }
}

namespace MeCab {

    bool RewritePattern::set_pattern(const char* src,
        const char* dst) {
        scoped_fixed_array<char, BUF_SIZE> buf;
        spat_.clear();
        dpat_.clear();

        strncpy_s(buf.get(), buf.size(), src, buf.size());
        tokenizeCSV(buf.get(), back_inserter(spat_), 512);

        strncpy_s(buf.get(), buf.size(), dst, buf.size());
        tokenizeCSV(buf.get(), back_inserter(dpat_), 512);

        return (spat_.size() && dpat_.size());
    }

    bool RewritePattern::rewrite(size_t size,
        const char** input,
        std::string* output) const {
        if (spat_.size() > size) return false;
        for (size_t i = 0; i < spat_.size(); ++i) {
            if (!match_rewrite_pattern(spat_[i].c_str(), input[i]))
                return false;
        }

        output->clear();
        for (size_t i = 0; i < dpat_.size(); ++i) {
            std::string elm;
            const char* begin = dpat_[i].c_str();
            const char* end = begin + dpat_[i].size();
            for (const char* p = begin; p < end; ++p) {
                if (*p == '$') {
                    ++p;
                    size_t n = 0;
                    for (; p < end; ++p) {
                        switch (*p) {
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            n = 10 * n + (*p - '0');
                            break;
                        default:
                            goto NEXT;
                        }
                    }
                NEXT:
                    CHECK_DIE(n > 0 && (n - 1) < size)
                        << " out of range: [" << dpat_[i] << "] " << n;
                    elm += input[n - 1];
                    if (p < end) elm += *p;
                }
                else {
                    elm += *p;
                }
            }
            CHECK_DIE(escape_csv_element(&elm));
            *output += elm;
            if (i + 1 != dpat_.size()) *output += ",";
        }

        return true;
    }

    bool RewriteRules::rewrite(size_t size,
        const char** input,
        std::string* output) const {
        for (size_t i = 0; i < this->size(); ++i) {
            if ((*this)[i].rewrite(size, input, output))
                return true;
        }
        return false;
    }

    void DictionaryRewriter::clear() { cache_.clear(); }

    bool DictionaryRewriter::open() {
        char unigramRewrite[] =
            "BOS/EOS,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,*,*,*,*,*,*,BOS/EOS,BOS/EOS,BOS/EOS,*,*,BOS/EOS,*,*,*\n"
            "*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$7,$8,$9,$11,$10,$12,$13,$25,$26,$27\n"
            "*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,*,*,*,*,*,*,*";

        char leftRewrite[] =
            "BOS/EOS,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,*,*,*,*,*,BOS/EOS,BOS/EOS,BOS/EOS,*,*,*,*,*,*\n"
            "助詞,*,*,*,*,*,*,(の|に|を|て|は|と|が|で|も|の|から|か|が|ね|か|けれど|など|って|と|ば|や|まで|へ|から|より|だけ|な|たり|よ|くらい|ながら|し|ほど|しか),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "助動詞,*,*,*,*,*,*,(だ|た|ます|です|れる|ず|ない|てる|られる|べし|たい|り|せる|ちゃう),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "名詞,助動詞語幹,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "形状詞,助動詞語幹,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "動詞,非自立可能,*,*,*,*,*,(為る|居る|有る|成る|見る|行く|来る|出来る|得る|遣る|仕舞う|呉れる|出す|置く|致す|付く|頂く|付ける|貰う|掛ける|続く|始める|続ける|御座る|終わる),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "動詞,一般,*,*,*,*,*,(於く),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "形容詞,非自立可能,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "接尾辞,*,*,*,*,*,*,(的|年|者|月|さん|日|パーセント|人|つ|等|日|円|等|化|達|人|さ|性|回|時|氏|所|生|方|分|長|党|目|中|省|歳|内|年度|国|家|後|部|上|車|権|度|力|員|費|書|用|物|型|業|間|メートル|庁|箇月|番|局|機|年間|館|件|時間|社),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "名詞,数詞,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "補助記号,*,*,*,*,*,*,*,．,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,*,*,$13,$16,$17,$18,$25,$26,$27\n"
            "*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,*,*,*,*,*,*";
        
        char rightRewrite[] =
            "BOS/EOS,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,*,*,*,*,*,BOS/EOS,BOS/EOS,BOS/EOS,*,*,*,*,*,*\n"
            "助詞,*,*,*,*,*,*,(の|に|を|て|は|と|が|で|も|の|から|か|が|ね|か|けれど|など|って|と|ば|や|まで|へ|から|より|だけ|な|たり|よ|くらい|ながら|し|ほど|しか),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "助動詞,*,*,*,*,*,*,(だ|た|ます|です|れる|ず|ない|てる|られる|べし|たい|り|せる|ちゃう),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "名詞,助動詞語幹,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "形状詞,助動詞語幹,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "動詞,非自立可能,*,*,*,*,*,(為る|居る|有る|成る|見る|行く|来る|出来る|得る|遣る|仕舞う|呉れる|出す|置く|致す|付く|頂く|付ける|貰う|掛ける|続く|始める|続ける|御座る|終わる),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "動詞,一般,*,*,*,*,*,(於く),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$16,$17,$18,$25,$26,$27\n"
            "形容詞,非自立可能,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "接尾辞,*,*,*,*,*,*,(的|年|者|月|さん|日|パーセント|人|つ|等|日|円|等|化|達|人|さ|性|回|時|氏|所|生|方|分|長|党|目|中|省|歳|内|年度|国|家|後|部|上|車|権|度|力|員|費|書|用|物|型|業|間|メートル|庁|箇月|番|局|機|年間|館|件|時間|社),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "接頭辞,*,*,*,*,*,*,(第|御|約|不|大|新|各|小|御|非),*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "名詞,数詞,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "補助記号,*,*,*,*,*,*,*,．,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,$9,$10,$13,$14,$15,$19,$25,$26,$27\n"
            "*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,*,*,$13,$14,$15,$19,$25,$26,$27\n"
            "*,*,*,*,*,*	$1,$2,$3,$4,$5,$6,*,*,*,*,*,*";
            
        append_rewrite_rule(&unigram_rewrite_, unigramRewrite);
        append_rewrite_rule(&left_rewrite_, leftRewrite);
        append_rewrite_rule(&right_rewrite_, rightRewrite);

        return true;
    }

    // without cache
    bool DictionaryRewriter::rewrite(const std::string& feature,
        std::string* ufeature,
        std::string* lfeature,
        std::string* rfeature) const {
        scoped_fixed_array<char, BUF_SIZE> buf;
        scoped_fixed_array<char*, BUF_SIZE> col;
        CHECK_DIE(feature.size() < buf.size() - 1) << "too long feature";
        strncpy_s(buf.get(), buf.size(), feature.c_str(), buf.size() - 1);
        const size_t n = tokenizeCSV(buf.get(), col.get(), col.size());
        CHECK_DIE(n < col.size()) << "too long CSV entities";
        return (unigram_rewrite_.rewrite(n, const_cast<const char**>(col.get()),
            ufeature) &&
            left_rewrite_.rewrite(n, const_cast<const char**>(col.get()),
                lfeature) &&
            right_rewrite_.rewrite(n, const_cast<const char**>(col.get()),
                rfeature));
    }

    // with cache
    bool DictionaryRewriter::rewrite2(const std::string& feature,
        std::string* ufeature,
        std::string* lfeature,
        std::string* rfeature) {
        std::map<std::string, FeatureSet>::const_iterator it = cache_.find(feature);
        if (it == cache_.end()) {
            if (!rewrite(feature, ufeature, lfeature, rfeature)) return false;
            FeatureSet f;
            f.ufeature = *ufeature;
            f.lfeature = *lfeature;
            f.rfeature = *rfeature;
            cache_.insert(std::pair<std::string, FeatureSet>(feature, f));
        }
        else {
            *ufeature = it->second.ufeature;
            *lfeature = it->second.lfeature;
            *rfeature = it->second.rfeature;
        }

        return true;
    }

    bool POSIDGenerator::open(const char* filename,
        Iconv* iconv) {
        std::ifstream ifs(WPATH(filename));
        if (!ifs) {
            std::cerr << filename
                << " is not found. minimum setting is used" << std::endl;
            rewrite_.resize(1);
            rewrite_.back().set_pattern("*", "1");
            return true;
        }

        std::string line;
        char* col[2];
        while (std::getline(ifs, line)) {
            if (iconv) iconv->convert(&line);
            const size_t n = tokenize2(const_cast<char*>(line.c_str()),
                " \t", col, 2);
            CHECK_DIE(n == 2) << "format error: " << line;
            for (char* p = col[1]; *p; ++p) {
                CHECK_DIE(*p >= '0' && *p <= '9') << "not a number: " << col[1];
            }
            rewrite_.resize(rewrite_.size() + 1);
            rewrite_.back().set_pattern(col[0], col[1]);
        }
        return true;
    }

    int POSIDGenerator::id(const char* feature) const {
        scoped_fixed_array<char, BUF_SIZE> buf;
        scoped_fixed_array<char*, BUF_SIZE> col;
        CHECK_DIE(std::strlen(feature) < buf.size() - 1) << "too long feature";
        strncpy_s(buf.get(), buf.size(), feature, buf.size() - 1);
        const size_t n = tokenizeCSV(buf.get(), col.get(), col.size());
        CHECK_DIE(n < col.size()) << "too long CSV entities";
        std::string tmp;
        if (!rewrite_.rewrite(n, const_cast<const char**>(col.get()), &tmp)) {
            return -1;
        }
        return std::atoi(tmp.c_str());
    }
}