#include "Word.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

/*!
    \fn Word::Word(const std::string& kana, const std::string& kanji, const std::string& workspace)

    Constructs a Word object from \a kana and \a kanji
    and reads word info from file \a workspace.
*/
Word::Word(const std::string& kana, const std::string& kanji, const std::string& workspace)
	: mKana(kana), mKanji(kanji), mWorkspace(workspace)
{
    readWord();
}

/*!
    \fn bool Word::addTextId(int64_t textId)

    Adds \a textId to the word info and writes to storage.
    Returns true if \a textId was not already in word info,
    otherwise returns false;
*/
bool Word::addTextId(int64_t textId)
{
    auto [iter, result] = mTextIds.insert(textId);
    if (result) {
        result = writeWord();
    }
    return result;
}

/*!
    \fn bool Word::removeTextId(int64_t textId)

    Removes \a textId from the word info and writes to storage.
    Returns true if erased, otherwise returns false;
*/
bool Word::removeTextId(int64_t textId)
{
    if (mTextIds.erase(textId) != 0) {
        return writeWord();
    }
    return false;
}

/*!
    \fn bool Word::addPitchAccent(uint8_t pitchAccent)

    Adds \a pitchAccent to the word info and writes to storage.
    Returns true if \a pitchAccent was not already in word info,
    otherwise returns false.
*/
bool Word::addPitchAccent(uint8_t pitchAccent)
{
    auto [iter, result] = mPitchAccents.insert(pitchAccent);
    if (result) {
        result = writeWord();
    }
    return result;
}

/*!
    \fn void Word::setWorkspace(const std::string& workspace)

    Sets the workspace to \a workspace, reads in word data from storage,
    and writes word data to storage.
*/
void Word::setWorkspace(const std::string& workspace)
{
    mWorkspace = workspace;
    readWord();
    writeWord();
}

/*!
    \fn const std::string& Word::getKana() const

    Returns kana of word.
*/
const std::string& Word::getKana() const
{
    return mKana;
}

/*!
    \fn const std::string& Word::getKanji() const

    Returns kanji of word.
*/
const std::string& Word::getKanji() const
{
    return mKanji;
}

/*!
    \fn const std::set<int64_t>& Word::getTextIds() const

    Returns text ids of texts associated with word.
*/
const std::set<int64_t>& Word::getTextIds() const
{
    return mTextIds;
}

/*!
    \fn const std::set<uint8_t>& Word::getPitchAccents() const

    Returns pitch accents of word.
*/
const std::set<uint8_t>& Word::getPitchAccents() const
{
    return mPitchAccents;
}

/*!
    \fn const std::string& Word::getNotes() const

    Returns notes associated with word.
*/
const std::string& Word::getNotes() const
{
    return mNotes;
}

/*!
    \fn void Word::setNotes(const std::string& notes)

    Sets notes data to \a notes.
*/
void Word::setNotes(const std::string& notes)
{
    mNotes = notes;
    writeWord();
}

/*!
    \fn bool Word::erase()

    Deletes word data from storage.
    Returns true if successful, otherwise returns false.
*/
bool Word::erase()
{
    if (mWorkspace.empty())
    {
        return false;
    }

    return QFile::remove(getFilePath());
}

/*!
    \fn void Word::readWord()

    Retrieves word data from storage.
*/
void Word::readWord()
{
    if (mWorkspace.empty()) {
        return;
    }

    QFile file(getFilePath());

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

    QJsonObject jsonObject = doc.object();
    if (jsonObject.contains("textIds") && jsonObject["textIds"].isArray()) {
        QJsonArray textIds = jsonObject["textIds"].toArray();
        for (QJsonValue val : textIds) {
            if (val.isDouble()) {
                mTextIds.insert(val.toInteger());
            }
        }
    }

    if (jsonObject.contains("pitchAccents") && jsonObject["pitchAccents"].isArray()) {
        QJsonArray pitchAccents = jsonObject["pitchAccents"].toArray();
        for (QJsonValue val : pitchAccents) {
            if (val.isDouble()) {
                mPitchAccents.insert(static_cast<uchar>(val.toVariant().toUInt()));
            }
        }
    }

    if (jsonObject.contains("notes") && jsonObject["notes"].isString()) {
        mNotes = jsonObject["notes"].toString().toStdString();
    }
}

/*!
    \fn bool Word::writeWord()

    Writes word data to storage.
*/
bool Word::writeWord()
{
    bool result = false;
    if (mWorkspace.empty()) {
        return true;
    }

    QFile file(getFilePath());

    if (!file.open(QIODevice::WriteOnly)) {
        return result;
    }

    QJsonObject jsonObject;
    QJsonArray jsonArray;
    if (!mTextIds.empty()) {
        for (qint64 textId : mTextIds) {
            jsonArray.append(QJsonValue(textId));
        }
        jsonObject.insert("textIds", QJsonValue(jsonArray));
    }

    if (!mPitchAccents.empty()) {
        jsonArray = QJsonArray();
        for (uchar pitchAccent : mPitchAccents) {
            jsonArray.append(QJsonValue(pitchAccent));
        }
        jsonObject.insert("pitchAccents", QJsonValue(jsonArray));
    }

    if (!mNotes.empty()) {
        jsonObject.insert("notes", mNotes.c_str());
    }

#ifdef QT_DEBUG
    QJsonDocument::JsonFormat format = QJsonDocument::Indented;
#else
    QJsonDocument::JsonFormat format = QJsonDocument::Compact;
#endif

    result = file.write(QJsonDocument(jsonObject).toJson(format)) > 0 ? true : false;
    file.close();
    return result;
}

/*!
    \fn QString Word::getFilePath()

    Returns path to word data file.
*/
QString Word::getFilePath()
{
    QDir dir = QDir(mWorkspace.c_str() + QString("/KonPonData") +
        QString("/Words") + QString("/") + QString(mKana.c_str())
    );
    if (!dir.exists()) {
        // Create folder if does not exist
        dir.mkpath(".");
    }
    return dir.absoluteFilePath(mKanji.c_str() + QString(".json"));
}
