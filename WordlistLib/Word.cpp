#include "Word.h"

#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

Word::Word(const std::string& kana, const std::string& kanji, const std::string& workspace)
	: mKana(kana), mKanji(kanji), mWorkspace(workspace)
{
    readWord();
}

bool Word::addTextId(int64_t textId)
{
    auto [iter, result] = mTextIds.insert(textId);
    if (result) {
        result = writeWord();
    }
    return result;
}

bool Word::removeTextId(int64_t textId)
{
    if (mTextIds.erase(textId) != 0) {
        return writeWord();
    }
    return false;
}

bool Word::addPitchAccent(uint8_t pitchAccent)
{
    auto [iter, result] = mPitchAccents.insert(pitchAccent);
    if (result) {
        result = writeWord();
    }
    return result;
}

void Word::setWorkspace(const std::string& workspace)
{
    mWorkspace = workspace;
    readWord();
    writeWord();
}

const std::string& Word::getKana() const
{
    return mKana;
}
const std::string& Word::getKanji() const
{
    return mKanji;
}

const std::set<int64_t>& Word::getTextIds() const
{
    return mTextIds;
}

const std::set<uint8_t>& Word::getPitchAccents() const
{
    return mPitchAccents;
}

const std::string& Word::getNotes() const
{
    return mNotes;
}

void Word::setNotes(const std::string& notes)
{
    mNotes = notes;
    writeWord();
}

bool Word::erase()
{
    if (mWorkspace.empty())
    {
        return false;
    }

    return QFile::remove(getFilePath());
}

// get word from file
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
