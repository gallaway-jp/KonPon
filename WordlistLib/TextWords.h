#pragma once

#include "wordlistlib_global.h"

#include <map>
#include <set>
#include <string>

/*!
	\class TextWords

	\brief The TextWords class provides methods
	manage word info related to a text.
*/
class WORDLISTLIB_EXPORT TextWords
{
	using Locations = std::map<size_t, uint8_t>; //value is length of word at location
	using WordDict = std::map<std::string, std::map<std::string, Locations>>;
	using LocationsDict = std::map<size_t, std::map<std::string, std::set<std::string>>>;
public: // interface methods
	TextWords(size_t fileId, const std::string& workspace);
	std::pair<std::string, std::string> getWordAt(size_t location);
	const WordDict& getWords() const { return mWords; }
	const LocationsDict& getLocations() const { return mLocations; }

private: // implementation fields
	size_t mFileId;
	std::string mWorkspace;
	WordDict mWords;
	LocationsDict mLocations;
private: // implementation methods
	void readWords();
};