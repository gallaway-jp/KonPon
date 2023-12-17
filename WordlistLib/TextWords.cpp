#include "TextWords.h"


#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

/*!
    \fn TextWords::TextWords(size_t fileId, const std::string& workspace)

    Constructs a TextWords object based on text data in \a workspace
    /KonPonData/Texts/ \a fileId / \a wordlist.json.
*/
TextWords::TextWords(size_t fileId, const std::string& workspace)
    : mFileId(fileId), mWorkspace(workspace), mWords(), mLocations()
{
    readWords();
}

std::pair<std::string, std::string> TextWords::getWordAt(size_t location)
{
    //if(mLocations.find()
    return {};
}

/*!
    \fn void TextWords::readWords()

    Gets wordlist data from file.
*/
void TextWords::readWords()
{
    if (!mWords.empty()) {
        return;
    }

    if (mWorkspace.empty()) {
        return;
    }

    QFile file(QDir(mWorkspace.c_str() + QString("/KonPonData") +
        QString("/Texts") + QString("/") + QString::number(mFileId)
    ).absoluteFilePath("wordlist.json"));

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return;
    }

    QJsonObject rootObj = doc.object();
    if (!rootObj.contains("words") || !rootObj.contains("locations")) {
        return;
    }

    QString teststring = doc.toVariant().toString();

    {
        QJsonObject wordsObj = rootObj["words"].toObject();
        QJsonObject::iterator iter;
        for (iter = wordsObj.begin();
            iter != wordsObj.end(); ++iter)
        {
            if (iter.value().isObject()) {
                QJsonObject::iterator kanjiIter;
                QJsonObject kanjiObject = iter.value().toObject();
                for (kanjiIter = kanjiObject.begin();
                    kanjiIter != kanjiObject.end(); ++kanjiIter) {
                    if (kanjiIter.value().isObject()) {
                        QJsonObject::iterator locationsIter;
                        QJsonObject locationsObject = kanjiIter.value().toObject();
                        for (locationsIter = locationsObject.begin();
                            locationsIter != locationsObject.end(); ++locationsIter) {
                            if (locationsIter.value().isDouble()) {
                                mWords[iter.key().toStdString()][kanjiIter.key().toStdString()][locationsIter.key().toULongLong()]
                                    = static_cast<uint8_t>(locationsIter.value().toInt());
                            }
                        }
                    }
                }
            }
        }
    }

    {
        QJsonObject locationsObj = rootObj["locations"].toObject();
        QJsonObject::iterator iter;
        for (iter = locationsObj.begin();
            iter != locationsObj.end(); ++iter)
        {
            if (iter.value().isObject()) {
                QJsonObject::iterator kanaIter;
                QJsonObject kanaObj = iter.value().toObject();
                for (kanaIter = kanaObj.begin();
                    kanaIter != kanaObj.end(); ++kanaIter) {
                    if (kanaIter.value().isArray()) {
                        for (const auto& kanji : kanaIter.value().toArray()) {
                            if (kanji.isString()) {
                                mLocations[iter.key().toULongLong()][kanaIter.key().toStdString()].insert(kanji.toString().toStdString());
                            }
                        }
                    }
                }
            }
        }
    }
}