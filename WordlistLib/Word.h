#pragma once

#include "wordlistlib_global.h"
#include <QString>

#include <set>
#include <string>


/*!
	\class Word

	\brief The Word class provides methods
	load and save word data.
*/
class WORDLISTLIB_EXPORT Word
{
public: //interface methods
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
	const std::string& getNotes() const;
	void setNotes(const std::string& notes);
	bool erase();

private: // implementation fields
	std::string mKana;
	std::string mKanji;
	std::string mWorkspace;
	std::set<int64_t> mTextIds;
	std::set<uint8_t> mPitchAccents;
	std::string mNotes;
private: // implementation methods
	void readWord();
	bool writeWord();
	QString getFilePath();
};