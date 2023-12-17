#include "Wordlists.h"

#include <stdexcept>

#include <QDir>
#include <QFileInfo>
#include <QString>

/*!
    \fn Wordlists::Wordlists(const std::string& workspace)

    Constructs a Wordlists object and initializes known and unknown lists.
*/
Wordlists::Wordlists(const std::string& workspace)
    : mWorkspace(workspace),
    mWordLists(std::map<std::string, Wordlist>()),
    mKnownWordlist(Wordlist("knownList", workspace, false)),
    mUnknownWordlist(Wordlist("unknownList", workspace, false))
{
    readCustomWordlists();
}

/*!
    \fn bool Wordlists::addCustomWordlist(const std::string& name)

    Adds custom wordlist if it does not already exists.
    Returns true if successful, otherwise returns false.
*/
bool Wordlists::addCustomWordlist(const std::string& name)
{
    const auto [iter, result] = mWordLists.try_emplace(name, Wordlist(name, mWorkspace));
    return result;
}

/*!
    \fn void Wordlists::removeCustomWordlist(const std::string& name)

    Removes \a name custom word list from Wordlists object.
*/
void Wordlists::removeCustomWordlist(const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    pos->second.erase();
    mWordLists.erase(pos);
}

/*!
    \fn void Wordlists::addWordsToUnknownWordlist(Wordlist& newWords)

    Adds \a newWords words to unknown wordlist.
*/
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

/*!
    \fn void Wordlists::addWordsToKnownWordlist(Wordlist& newWords)

    Adds \a newWords words to known wordlist.
*/
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

/*!
    \fn void Wordlists::addWordsToCustomWordlist(Wordlist& newWords, const std::string& name)

    Adds \a newWords words to \a name custom wordlist.
*/
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

/*!
    \fn void Wordlists::moveWordsToUnknownWordlist(const Wordlist& newWords, const std::string& sourceCustomList)

    Moves \a newWords from previous word list to unknown word list.
*/
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

/*!
    \fn void Wordlists::moveWordsToKnownWordlist(const Wordlist& newWords, const std::string& sourceCustomList)

    Moves \a newWords from previous word list to known word list.
*/
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

/*!
    \fn void Wordlists::moveWordsToCustomWordlist(const Wordlist& newWords, const std::string& name, const std::string& sourceCustomList)

    Moves \a newWords from previous word list to \a name custom word list.
*/
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

/*!
    \fn void Wordlists::removeWordFromUnknownWordlist(const std::string& kana, const std::string& kanji)

    Removes word of \a kana and \a kanji from unknown word list.
*/
void Wordlists::removeWordFromUnknownWordlist(const std::string& kana, const std::string& kanji)
{
    mUnknownWordlist.removeWord(kana, kanji);
    mUnknownWordlist.removeWords(Wordlist()); // Save to file
}

/*!
    \fn void Wordlists::removeWordFromKnownWordlist(const std::string& kana, const std::string& kanji)

    Removes word of \a kana and \a kanji from known word list.
*/
void Wordlists::removeWordFromKnownWordlist(const std::string& kana, const std::string& kanji)
{
    mKnownWordlist.removeWord(kana, kanji);
    mKnownWordlist.removeWords(Wordlist()); // Save to file
}

/*!
    \fn void Wordlists::removeWordFromCustomWordlist(const std::string& kana, const std::string& kanji, const std::string& name)

    Removes word of \a kana and \a kanji from \a name custom word list.
*/
void Wordlists::removeWordFromCustomWordlist(const std::string& kana, const std::string& kanji, const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    pos->second.removeWord(kana, kanji);
    pos->second.removeWords(Wordlist()); // Save to file
}

/*!
    \fn void Wordlists::removeWordsFromUnknownWordlist(const Wordlist& wordsToRemove)

    Removes \a wordsToRemove words from unknown word list.
*/
void Wordlists::removeWordsFromUnknownWordlist(const Wordlist& wordsToRemove)
{
    mUnknownWordlist.removeWords(wordsToRemove);
}

/*!
    \fn void Wordlists::removeWordsFromKnownWordlist(const Wordlist& wordsToRemove)

    Removes \a wordsToRemove words from known word list.
*/
void Wordlists::removeWordsFromKnownWordlist(const Wordlist& wordsToRemove)
{
    mKnownWordlist.removeWords(wordsToRemove);
}

/*!
    \fn void Wordlists::removeWordsFromCustomWordlist(const Wordlist& wordsToRemove, const std::string& name)

    Removes \a wordsToRemove words from \a name custom word list.
*/
void Wordlists::removeWordsFromCustomWordlist(const Wordlist& wordsToRemove, const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return;
    }

    pos->second.removeWords(wordsToRemove);
}

/*!
    \fn const Wordlist& Wordlists::getCustomWordlist(const std::string& name)

    Returns the \a name custom word list.
    If custom word list does not exist, throws an "Invalid Name of Custom Wordlist" exception.
*/
const Wordlist& Wordlists::getCustomWordlist(const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        throw std::invalid_argument("Invalid Name of Custom Wordlist");
    }
    
    return pos->second;
}

/*!
    \fn const Wordlist& Wordlists::getUnknownWordlist()

    Returns the unknown word list.
*/
const Wordlist& Wordlists::getUnknownWordlist()
{
    return mUnknownWordlist;
}

/*!
    \fn const Wordlist& Wordlists::getKnownWordlist()

    Returns the known word list.
*/
const Wordlist& Wordlists::getKnownWordlist()
{
    return mKnownWordlist;
}

/*!
    \fn bool Wordlists::customWordlistExists(const std::string& name)

    Returns true if \a name custom word list exists, otherwise returns false.
*/
bool Wordlists::customWordlistExists(const std::string& name)
{
    auto pos = mWordLists.find(name);
    if (pos == mWordLists.end()) {
        return false;
    }

    return true;
}

/*!
    \fn std::vector<std::string> Wordlists::getCustomWordlistNames()

    Returns vector of custom word list names.
*/
std::vector<std::string> Wordlists::getCustomWordlistNames()
{
    std::vector<std::string> keys;

    for (const auto& [key, _] : mWordLists) {
        keys.push_back(key);
    }
    return keys;
}

/*!
    \fn void Wordlists::readCustomWordlists()

    Loads custom word lists data from storage.
*/
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

/*!
    \fn void Wordlists::setUnknownColor(WordListInfo::Color color)

    Sets the unknown word list's color to \a color.
*/
void Wordlists::setUnknownColor(WordListInfo::Color color)
{
    mUnknownWordlist.setColor(color);
}

/*!
    \fn void Wordlists::setKnownColor(WordListInfo::Color color)

    Sets the known word list's color to \a color.
*/
void Wordlists::setKnownColor(WordListInfo::Color color)
{
    mKnownWordlist.setColor(color);
}

/*!
    \fn void Wordlists::setCustomColor(const std::string& wordlist, WordListInfo::Color color)

    Sets the \a wordlist custom word list's color to \a color.
*/
void Wordlists::setCustomColor(const std::string& wordlist, WordListInfo::Color color)
{
    if (mWordLists.find(wordlist) == mWordLists.end()) {
        return;
    }
    mWordLists[wordlist].setColor(color);
}

/*!
    \fn bool Wordlists::contains(const std::string& kana, const std::string& kanji)

    Returns true if any word list contains word of \a kana and \a kanji.
*/
bool Wordlists::contains(const std::string& kana, const std::string& kanji)
{
    bool wordExists = false;
    wordExists = mUnknownWordlist.contains(kana, kanji)
        || mKnownWordlist.contains(kana, kanji);

    if (wordExists) {
        return wordExists;
    }

    for (const auto& [_,customWordlist] : mWordLists) {
        wordExists = customWordlist.contains(kana, kanji);
        if (wordExists) {
            return wordExists;
        }
    }

    return wordExists;
}