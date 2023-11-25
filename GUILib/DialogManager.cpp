#include "DialogManager.h"

#include "Settings.h"
#include "Wordlists.h"
#include "WordView.h"
#include "TextView.h"

#include <QDialog>
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

DialogManager::DialogManager(Settings* settings, Wordlists& wordlists)
	: mSettings(settings), mWordlists(wordlists)
{
    readTextIdsFromJsonFile();
}

QString DialogManager::getTextIdsJsonPath()
{
    return QDir(mSettings->mFile.workspace + QString("/KonPonData")).absoluteFilePath("TextIds.json");
}

bool DialogManager::readTextIdsFromJsonFile()
{
    QFile file(getTextIdsJsonPath());

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonObject obj = doc.object();
    auto iter = obj.constBegin();
    auto iterEnd = obj.constEnd();
    while (iter != iterEnd) {
        if (iter.value().isString()) {
            QString val = iter.value().toString();
            mTextIds[iter.key().toLongLong()].first = val.toStdString();
            mTextIds[iter.key().toLongLong()].second = nullptr;
        }
        iter++;
    }

    return true;
}

bool DialogManager::writeTextIdsToJsonFile()
{
    QFile file(getTextIdsJsonPath());
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject object;
    for (const auto& [textId, valPair] : mTextIds) {
        const auto& [name, _] = valPair;
        object[QString::number(textId)] = name.c_str();
    }

    bool writeSucceeded = file.write(QJsonDocument(object).toJson()) > 0;
    file.close();
    return writeSucceeded;
}

void DialogManager::onViewTextClicked(int64_t textId)
{
    if (mTextIds[textId].second == nullptr) {
        // Open the textview for the selected file
        TextView* textView = new TextView(nullptr, textId, mSettings, mWordlists);
        setTextOpen(textId, dynamic_cast<QDialog*>(textView));
        connect(textView, &TextView::closeDialog, this, [this, textId]() {setTextOpen(textId); });
        connect(textView, &TextView::viewWordClicked, this, &DialogManager::onViewWordClicked);
    }
    mTextIds[textId].second->show();
    mTextIds[textId].second->raise();
    mTextIds[textId].second->activateWindow();
}

void DialogManager::setTextOpen(int64_t textId, QDialog* dialog)
{
    mTextIds[textId].second = dialog;
}

void DialogManager::onAddTextId(int64_t textId, const std::string& name)
{
    mTextIds[textId].first = name;
    mTextIds[textId].second = nullptr;
    writeTextIdsToJsonFile();
}

void DialogManager::onRemoveTextId(int64_t textId)
{
    mTextIds.erase(textId);
    emit removeTextIdFromWordInfo(textId);
    writeTextIdsToJsonFile();
}

void DialogManager::onViewWordClicked(const std::string& kana, const std::string& kanji)
{
    if (mWordMap[{kana, kanji}] == nullptr) {
        // Open the wordview for the selected word
        WordView* wordView = new WordView(kana, kanji, mSettings, mWordlists, mTextIds);
        setWordOpen(kana, kanji, dynamic_cast<QDialog*>(wordView));
        connect(wordView, &WordView::closeDialog, this, [this, kana, kanji]() {setWordOpen(kana, kanji); });
        connect(wordView, &WordView::viewTextClicked, this, &DialogManager::onViewTextClicked);
        connect(this, &DialogManager::removeTextIdFromWordInfo, wordView, &WordView::onRemoveTextIdFromWordInfo);
    }
    mWordMap[{kana, kanji}]->show();
    mWordMap[{kana, kanji}]->raise();
    mWordMap[{kana, kanji}]->activateWindow();
}

void DialogManager::setWordOpen(const std::string& kana, const std::string& kanji, QDialog* dialog)
{
    mWordMap[{kana, kanji}] = dialog;
}