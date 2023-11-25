#pragma once

#include "wordlistlib_global.h"
#include "Wordlist.h"

#include <map>
#include <string>
#include <vector>

class WORDLISTLIB_EXPORT Wordlists
{
public: // interface methods
    Wordlists(const std::string& workspace);
    bool addCustomWordlist(const std::string& name);
    void removeCustomWordlist(const std::string& name);
    void addWordsToUnknownWordlist(Wordlist& newWords);
    void addWordsToKnownWordlist(Wordlist& newWords);
    void addWordsToCustomWordlist(Wordlist& newWords, const std::string& name);
    void moveWordsToUnknownWordlist(const Wordlist& newWords, const std::string& sourceCustomList = "");
    void moveWordsToKnownWordlist(const Wordlist& newWords, const std::string& sourceCustomList = "");
    void moveWordsToCustomWordlist(const Wordlist& newWords, const std::string& name, const std::string& sourceCustomList = "");
    void removeWordFromUnknownWordlist(const std::string& kana, const std::string& kanji);
    void removeWordFromKnownWordlist(const std::string& kana, const std::string& kanji);
    void removeWordFromCustomWordlist(const std::string& kana, const std::string& kanji, const std::string& name);
    void removeWordsFromUnknownWordlist(const Wordlist& wordsToRemove);
    void removeWordsFromKnownWordlist(const Wordlist& wordsToRemove);
    void removeWordsFromCustomWordlist(const Wordlist& wordsToRemove, const std::string& name);
    const Wordlist& getCustomWordlist(const std::string& name);
    const Wordlist& getUnknownWordlist();
    const Wordlist& getKnownWordlist();
    bool customWordlistExists(const std::string& name);
    std::vector<std::string> getCustomWordlistNames();

    void setUnknownColor(WordListInfo::Color color);
    void setKnownColor(WordListInfo::Color color);
    void setCustomColor(const std::string& wordlist, WordListInfo::Color color);
public: // interface member variables
    
private: // implementation member variables
    std::string mWorkspace;
    std::map<std::string, Wordlist> mWordLists;
    Wordlist mKnownWordlist;
    Wordlist mUnknownWordlist;
private: // implementation methods
    void readCustomWordlists();
};
