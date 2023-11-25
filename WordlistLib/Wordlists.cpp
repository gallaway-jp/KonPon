#include "Wordlists.h"

#include <stdexcept>

#include <QString>
#include <QDir>
#include <QFileInfo>

Wordlists::Wordlists(const std::string& workspace)
    : mWorkspace(workspace),
    mWordLists(std::map<std::string, Wordlist>()),
    mKnownWordlist(Wordlist("knownList", workspace, false)),
    mUnknownWordlist(Wordlist("unknownList", workspace, false))
{
    readCustomWordlists();
}

// Adds custom wordlist if it does not already exists
// Returns true if successful, otherwise returns false
bool Wordlists::addCustomWordlist(const std::string& name)
{
    const auto [iter, result] = mWordLists.try_emplace(name, Wordlist(name, mWorkspace));
    return result;
}

void Wordlists::removeCustomWordlist(const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    pos->second.erase();
    mWordLists.erase(pos);
}

void Wordlists::addWordsToUnknownWordlist(Wordlist& newWords)
{
    mKnownWordlist.getUniqueItems(newWords);
    for (auto & [name, wordlist] : mWordLists) {
        wordlist.getUniqueItems(newWords);
    }
    if (!newWords.empty()) {
        mUnknownWordlist.insertWords(newWords);
    }
}

void Wordlists::addWordsToKnownWordlist(Wordlist& newWords)
{
    mUnknownWordlist.getUniqueItems(newWords);
    for (auto& [name, wordlist] : mWordLists) {
        wordlist.getUniqueItems(newWords);
    }
    if (!newWords.empty()) {
        mKnownWordlist.insertWords(newWords);
    }
}

void Wordlists::addWordsToCustomWordlist(Wordlist& newWords, const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    mUnknownWordlist.getUniqueItems(newWords);
    mKnownWordlist.getUniqueItems(newWords);
    for (auto& [listName, wordlist] : mWordLists) {
        if (listName == name) {
            continue;
        }
        wordlist.getUniqueItems(newWords);
    }
    if (!newWords.empty()) {
        pos->second.insertWords(newWords);
    }
}

void Wordlists::moveWordsToUnknownWordlist(const Wordlist& newWords, const std::string& sourceCustomList)
{
    mKnownWordlist.removeWords(newWords);
    for (auto& [name, wordlist] : mWordLists) {
        if (name == sourceCustomList) {
            continue;
        }
        wordlist.removeWords(newWords);
    }
    mUnknownWordlist.insertWords(newWords);
}

void Wordlists::moveWordsToKnownWordlist(const Wordlist& newWords, const std::string& sourceCustomList)
{
    mUnknownWordlist.removeWords(newWords);
    for (auto& [name, wordlist] : mWordLists) {
        if (name == sourceCustomList) {
            continue;
        }
        wordlist.removeWords(newWords);
    }
    mKnownWordlist.insertWords(newWords);
}

void Wordlists::moveWordsToCustomWordlist(const Wordlist& newWords, const std::string& name, const std::string& sourceCustomList)
{
    if (name == sourceCustomList) {
        return;
    }
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    mUnknownWordlist.removeWords(newWords);
    mKnownWordlist.removeWords(newWords);
    for (auto& [listName, wordlist] : mWordLists) {
        if (listName == name) {
            continue;
        }
        if (listName == sourceCustomList) {
            continue;
        }
        wordlist.removeWords(newWords);
    }
    pos->second.insertWords(newWords);
}

void Wordlists::removeWordFromUnknownWordlist(const std::string& kana, const std::string& kanji)
{
    mUnknownWordlist.removeWord(kana, kanji);
}

void Wordlists::removeWordFromKnownWordlist(const std::string& kana, const std::string& kanji)
{
    mKnownWordlist.removeWord(kana, kanji);
}

void Wordlists::removeWordFromCustomWordlist(const std::string& kana, const std::string& kanji, const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    pos->second.removeWord(kana, kanji);
}

void Wordlists::removeWordsFromUnknownWordlist(const Wordlist& wordsToRemove)
{
    mUnknownWordlist.removeWords(wordsToRemove);
}

void Wordlists::removeWordsFromKnownWordlist(const Wordlist& wordsToRemove)
{
    mKnownWordlist.removeWords(wordsToRemove);
}

void Wordlists::removeWordsFromCustomWordlist(const Wordlist& wordsToRemove, const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    pos->second.removeWords(wordsToRemove);
}

const Wordlist& Wordlists::getCustomWordlist(const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        throw std::invalid_argument("Invalid Name of Custom Wordlist");
    }
    
    return pos->second;
}

const Wordlist& Wordlists::getUnknownWordlist()
{
    return mUnknownWordlist;
}
const Wordlist& Wordlists::getKnownWordlist()
{
    return mKnownWordlist;
}

bool Wordlists::customWordlistExists(const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return false;
    }

    return true;
}

std::vector<std::string> Wordlists::getCustomWordlistNames()
{
    std::vector<std::string> keys;

    for (const auto& [key, _] : mWordLists) {
        keys.push_back(key);
    }
    return keys;
}

void Wordlists::readCustomWordlists()
{
    if (!mWordLists.empty()) {
        return;
    }

    if (mWorkspace.empty()) {
        return;
    }

    auto customWordlistNames = QDir(mWorkspace.c_str() + QString("/KonPonData") + QString("/CustomWordlists")).entryList({ "*.json" }, QDir::Files);
    for (auto& customWordlistName : customWordlistNames) {
        customWordlistName = QFileInfo(customWordlistName).completeBaseName();
        mWordLists.try_emplace(customWordlistName.toStdString(), Wordlist(customWordlistName.toStdString(), mWorkspace));
    }
}

void Wordlists::setUnknownColor(WordListInfo::Color color)
{
    mUnknownWordlist.setColor(color);
}

void Wordlists::setKnownColor(WordListInfo::Color color)
{
    mKnownWordlist.setColor(color);
}

void Wordlists::setCustomColor(const std::string& wordlist, WordListInfo::Color color)
{
    if (mWordLists.find(wordlist) == mWordLists.end()) {
        return;
    }
    mWordLists[wordlist].setColor(color);
}