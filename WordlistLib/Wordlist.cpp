#include "Wordlist.h"

#include <algorithm>

#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

const std::map<WordListInfo::Color, std::pair<int, int>> WordListInfo::Colors = {
        {Color::NOCOLOR, {Qt::transparent, Qt::transparent}},
        {Color::GRAY, {Qt::lightGray, Qt::darkGray}},
        {Color::RED, {Qt::red, Qt::darkRed}},
        {Color::GREEN, {Qt::green, Qt::darkGreen}},
        {Color::BLUE, {Qt::blue, Qt::darkBlue}},
        {Color::YELLOW, {Qt::yellow, Qt::darkYellow}},
        {Color::CYAN, {Qt::cyan, Qt::darkCyan}},
        {Color::MAGENTA, {Qt::magenta, Qt::darkMagenta}}
};

Wordlist::Wordlist(const std::string& name, const std::string& workspace, bool isCustomList)
	: mName(name), mWorkspace(workspace), mIsCustomList(isCustomList),
    mWords(std::multimap<std::string, std::string>())
{
    readWords();
}

// For testing purposes
Wordlist::Wordlist()
    : Wordlist("", "")
{
    ;
}

WordListInfo::Color Wordlist::getColor() const
{
    return mColor;
}

void Wordlist::setColor(WordListInfo::Color color)
{
    if (color != mColor) {
        mColor = color;
        writeWords();
    }
}

// Returns true if there are no words in list
bool Wordlist::empty()
{
    return mWords.empty();
}

// Returns count of word in list
size_t Wordlist::size()
{
    return mWords.size();
}

bool Wordlist::contains(const std::string& kana, const std::string& kanji) const
{
    auto i1 = mWords.equal_range(kana);
    for (auto& i2 = i1.first; i2 != i1.second; ++i2)
    {
        if (i2->second == kanji) {
            return true;
        }
    }
    return false;
}

// Rename wordlist, creates new file, and erases old file
// Returns true if successful, otherwise reverts to old name and returns false
bool Wordlist::rename(const std::string& name)
{
    bool result = false;

    result = erase();
    if (!result) {
        // failed to erase old file
        return result;
    }

    std::string oldString = std::move(mName);
    mName = name;
    result =  writeWords();
    if (!result) {
        // failed to create new file, so revert to old name
        mName = std::move(oldString);
        writeWords();
    }
    return result;
}

// Modifies newWords map to only contain unique items compared to this map
void Wordlist::getUniqueItems(Wordlist& newWords) const
{
    std::multimap<std::string, std::string> temp;
    std::set_difference(std::make_move_iterator(newWords.mWords.begin()),
        std::make_move_iterator(newWords.mWords.end()),
        mWords.begin(), mWords.end(),
        std::inserter(temp, temp.end()));
    newWords.mWords.swap(temp);
}

// Add word to this map
bool Wordlist::insertWord(const std::string& kana, const std::string& kanji)
{
    auto i1 = mWords.equal_range(kana);
    for (auto& i2 = i1.first; i2 != i1.second; ++i2)
    {
        if (i2->second == kanji) {
            // Word already exists in map
            return false;
        }
        if (i2->second > kanji) {
            mWords.emplace_hint(i2, kana, kanji);
            return true;
        }
    }
    // Kana does not already exist, so add normally
    mWords.emplace(kana, kanji);
    return true;
}

// Add words from newWords to this map
void Wordlist::insertWords(const Wordlist& newWords)
{
    for (auto const& [kana, kanji] : newWords.mWords) {
        insertWord(kana, kanji);
    }
    writeWords();
}

// removes word from this map
void Wordlist::removeWord(const std::string& kana, const std::string& kanji)
{
    auto i1 = mWords.equal_range(kana);
    for (auto& i2 = i1.first; i2 != i1.second; ++i2)
    {
        if (i2->second == kanji) {
            mWords.erase(i2);
            return;
        }
    }
}

// removes words in wordsToRemove from this map
void Wordlist::removeWords(const Wordlist& wordsToRemove)
{
    for (auto const& [kana, kanji] : wordsToRemove.mWords) {
        removeWord(kana, kanji);
    }
    writeWords();
}

// Delete wordlist file
bool Wordlist::erase()
{
    if (mWorkspace.empty()) {
        return false;
    }

    return QFile::remove(QDir(mWorkspace.c_str() + QString("/KonPonData") +
        QString(mIsCustomList ? "/CustomWordlists" : "")
    ).absoluteFilePath(mName.c_str() + QString(".json")));
}

// get wordlist from file
void Wordlist::readWords()
{
    if (!mWords.empty()) {
        return;
    }

    if (mWorkspace.empty()) {
        return;
    }

    QDir dir = QDir(mWorkspace.c_str() + QString("/KonPonData") +
        QString(mIsCustomList ? "/CustomWordlists" : ""));
    QFile file(dir.absoluteFilePath(mName.c_str() + QString(".json")));

    if (!file.exists()) {
        dir.mkpath(".");
        file.open(QIODevice::WriteOnly);
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return;
    }

    if (!doc.isObject()) {
        return;
    }

    QJsonObject rootObj = doc.object();
    if (rootObj.contains("color")) {
        mColor = static_cast<WordListInfo::Color>(rootObj["color"].toInt());
    }

    if (rootObj.contains("words")) {
        QJsonObject wordsObj = rootObj["words"].toObject();
        QJsonObject::iterator iter;
        for (iter = wordsObj.begin();
            iter != wordsObj.end(); ++iter)
        {
            if (iter.value().isString()) {
                mWords.insert(std::pair<std::string, std::string>
                    (iter.key().toStdString(), iter.value().toString().toStdString())
                );
            }
            else if (iter.value().isArray()) {
                for (QJsonValue value : iter.value().toArray()) {
                    if (value.isString()) {
                        mWords.insert(std::pair<std::string, std::string>
                            (iter.key().toStdString(), value.toString().toStdString())
                        );
                    }
                }
            }
        }
    }
}

bool Wordlist::writeWords()
{
    bool result = false;
    if (mWorkspace.empty()) {
        return result;
    }

    QFile file(QDir(mWorkspace.c_str() + QString("/KonPonData") +
        QString(mIsCustomList?"/CustomWordlists":"")
    ).absoluteFilePath(mName.c_str() + QString(".json")));

    if (!file.open(QIODevice::WriteOnly)) {
        return result;
    }
    
    QJsonObject object;
    QJsonObject wordsObject;   
    for (const auto& [key, value] : mWords) {
        QString kana(key.c_str());
        QString kanji(value.c_str());
        if (wordsObject.contains(kana)) {
            QJsonValue kanjiValue = wordsObject[kana];
            if (kanjiValue.isString()) {
                QString existingKanji(kanjiValue.toString());
                wordsObject[kana] = QJsonArray{ QJsonValue(existingKanji), QJsonValue(kanji) };
            }
            else if (kanjiValue.isArray()) {
                QJsonArray kanjiArray = kanjiValue.toArray();
                kanjiArray.append(QJsonValue(kanji));
                wordsObject[kana] = kanjiArray;
            }
            continue;
        }
        wordsObject[kana] = QJsonValue(kanji);
    }
    object["words"] = wordsObject;
    object["color"] = static_cast<int>(mColor);

#ifdef QT_DEBUG
    QJsonDocument::JsonFormat format = QJsonDocument::Indented;
#else
    QJsonDocument::JsonFormat format = QJsonDocument::Compact;
#endif

    result = file.write(QJsonDocument(object).toJson(format)) > 0 ? true : false;
    file.close();
    return result;
}