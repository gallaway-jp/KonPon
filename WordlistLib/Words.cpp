#include "Words.h"

#include "Word.h"
bool Words::insertWord(const Word& newWord)
{	
	auto iter = mWords.find({ newWord.getKana(), newWord.getKanji() });
	if (iter == mWords.end()) {
		mWords[{ newWord.getKana(), newWord.getKanji() }] = newWord;
		return true;
	}
	for (uint8_t newPitchAccent : newWord.getPitchAccents()) {
		mWords[{newWord.getKana(), newWord.getKanji()}].addPitchAccent(newPitchAccent);
	}
	for (uint64_t newPitchAccent : newWord.getTextIds()) {
		mWords[{newWord.getKana(), newWord.getKanji()}].addTextId(newPitchAccent);
	}
	return true;
}

bool Words::setWorkspace(const std::string& workspace)
{
	for (auto [key, val] : mWords) {
		val.setWorkspace(workspace);
	}

	return true;
}