#pragma once

#include "ParserTypes.h"

#include <String>

class Wordlist;
class Words;

namespace ParserFunc {
	std::pair<WordDict, LocationsDict> Tokenize(std::string const& folder, const char* pInputText, const std::string& fileId, Wordlist& outWordlist, Words& outWords);
}