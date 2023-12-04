#pragma once
#include "Util.h"

#include "unordered_map"

#include <QObject>
#include <QString>

class Settings;
class Wordlists;

class QDialog;

class DialogManager : public QObject
{
	using TextIdsMap = std::unordered_map<int64_t, std::pair<std::string, QDialog*>>;
	using WordMap = std::unordered_map <std::pair<std::string, std::string>, QDialog*, WordMapHashFunction>;
	Q_OBJECT
public:
	DialogManager(Settings* settings, Wordlists& wordlists);

public slots:
	void onViewTextClicked(int64_t textId);
	void setTextOpen(int64_t textId, QDialog* dialog = nullptr);
	void onAddTextId(int64_t textId, const std::string& name);
	void onRemoveTextId(int64_t textId);

	void onViewWordClicked(const std::string& kana, const std::string& kanji);
	void setWordOpen(const std::string& kana, const std::string& kanji, QDialog* dialog = nullptr);
signals:
	void removeTextId(int64_t textId);

private:
	Settings* mSettings = nullptr;
	Wordlists& mWordlists;
	TextIdsMap mTextIds;
	WordMap mWordMap;
private:
	QString getTextIdsJsonPath();
	bool readTextIdsFromJsonFile();
	bool writeTextIdsToJsonFile();
};