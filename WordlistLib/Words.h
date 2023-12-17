#pragma once

#include "Word.h"

#include <string>
#include <map>

/*!
	\class Words

	\brief The Words class provides methods
	to edit info associated with a word.
*/
class WORDLISTLIB_EXPORT Words
{
	using MapType = std::map<std::pair<std::string, std::string>, Word>;
public: // interface methods
	QString getPath();
	bool insertWord(const Word& newWord);
	bool setWorkspace(const std::string& workspace);
	void removeTextId(int64_t textId);
	MapType::const_iterator begin() const { return mWords.begin(); }
	MapType::const_iterator end() const { return mWords.end(); }

private: // implementation fields
	 MapType mWords;
	 std::string mWorkspace;
};