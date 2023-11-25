#pragma once

#include "ParserTypes.h"

#include <String>

class Wordlist;
class Words;

std::pair<WordDict, LocationsDict> Tokenize(std::string const& folder, const char* pInputText, const std::string& fileId, Wordlist& outWordlist, Words& outWords);