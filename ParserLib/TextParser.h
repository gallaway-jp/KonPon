#pragma once

#include "parserlib_global.h"

#include <string>
#include <vector>

class Wordlist;
class Words;

namespace Parser {
	int64_t PARSERLIB_EXPORT TokenizeText(std::string const& folder, std::string const& name, const char* pInputText,
		const std::string& fileId, Wordlist& outWordlist, Words& outWords);
};