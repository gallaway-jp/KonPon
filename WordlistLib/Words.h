#pragma once

#include "wordlistlib_global.h"
#include "Word.h"

#include <string>
#include <map>

class WORDLISTLIB_EXPORT Words
{
	using MapType = std::map<std::pair<std::string, std::string>, Word>;
public:
	bool insertWord(const Word& newWord);
	bool setWorkspace(const std::string& workspace);
	MapType::const_iterator begin() const { return mWords.begin(); }
	MapType::const_iterator end() const { return mWords.end(); }
private:
	 MapType mWords;
};