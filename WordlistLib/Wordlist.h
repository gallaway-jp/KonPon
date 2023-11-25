#pragma once

#include "wordlistlib_global.h"
#include <map>
#include <string>

#include <qnamespace.h>
struct WORDLISTLIB_EXPORT WordListInfo {
    enum class Color {
        NOCOLOR,
        GRAY,
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CYAN,
        MAGENTA,
    };

    enum class Type {
        UNDEFINED,
        UNKNOWN,
        KNOWN,
        CUSTOM
    };

    static const std::map<Color, std::pair<int, int>> Colors;
};

class WORDLISTLIB_EXPORT Wordlist
{
    using MultiMapType = std::multimap<std::string, std::string>;
public: // interface methods
    Wordlist(const std::string& name, const std::string& workspace, bool isCustomList = true);
    Wordlist();
    WordListInfo::Color getColor() const;
    void setColor(WordListInfo::Color color);
    bool empty();
    size_t size();
    bool contains(const std::string& kana, const std::string& kanji) const;
    bool rename(const std::string& name);
    void getUniqueItems(Wordlist& newWords) const;
    bool insertWord(const std::string& kana, const std::string& kanji);
    void insertWords(const Wordlist& newWords);
    void removeWord(const std::string& kana, const std::string& kanji);
    void removeWords(const Wordlist& wordsToRemove);
    bool erase();
public: // interface member variables
    MultiMapType::const_iterator begin() const { return mWords.begin(); }
    MultiMapType::const_iterator end() const { return mWords.end(); }
private: // implementation member variables
    MultiMapType mWords;
    std::string mName;
    std::string mWorkspace;
    bool mIsCustomList;
    WordListInfo::Color mColor = WordListInfo::Color::NOCOLOR;
private: // implementation methods
    void readWords();
    bool writeWords();
};
