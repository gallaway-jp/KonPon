#pragma once

#include "wordlistlib_global.h"
#include <string>
#include <set>

#include <QString>

class WORDLISTLIB_EXPORT Word
{
public:
	Word() = default;
	Word(const std::string& kana, const std::string& kanji, const std::string& workspace = "");
	bool addTextId(int64_t textId);
	bool removeTextId(int64_t textId);
	bool addPitchAccent(uint8_t pitchAccent);
	void setWorkspace(const std::string& workspace);
	const std::string& getKana() const;
	const std::string& getKanji() const;
	const std::set<int64_t>& getTextIds() const;
	const std::set<uint8_t>& getPitchAccents() const;
	bool erase();
private:
	std::string mKana;
	std::string mKanji;
	std::string mWorkspace;
	std::set<int64_t> mTextIds;
	std::set<uint8_t> mPitchAccents;

private: // implementation methods
	void readWord();
	bool writeWord();
	QString getFilePath();
};