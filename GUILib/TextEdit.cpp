#include "TextEdit.h"

#include "TextWords.h"

#include <QMouseEvent>


TextEdit::TextEdit(QWidget* parent, TextWords& textWords, const int64_t& offset, const qsizetype& currentPage)
	: QTextEdit(parent), mTextWords(textWords), mOffset(offset), mCurrentPage(currentPage)
{
	setReadOnly(true);
	setMouseTracking(true);


}

void TextEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
	QTextCursor tc = cursorForPosition(event->pos());
	int wordPosition = tc.position();
	
	auto word = getWordAtPosition(wordPosition);

	if (!word.first.empty() && !word.second.empty()) {
		emit selectedWord(word);
	}
}

void TextEdit::mouseMoveEvent(QMouseEvent* event)
{
	QTextCursor tc = cursorForPosition(event->pos());
	int wordPosition = tc.position();
	auto word = getWordAtPosition(wordPosition);
	
	emit hoveredWord(word, event->globalPos());
	QTextEdit::mouseMoveEvent(event);
}

std::pair<std::string, std::string> TextEdit::getWordAtPosition(int wordPosition)
{
	std::pair<std::string, std::string> result;
	const auto& locations = mTextWords.getLocations();
	int64_t location = wordPosition + mOffset;
	auto it = locations.lower_bound(location);

	if (it == locations.end()) {
		return result;
	}

	if (it->first > location) {
		if (it != locations.begin()) {
			--it;
		}
	}

	const auto& wordsDict = mTextWords.getWords();

	const auto& kana = it->second.begin()->first;
	const auto& kanji = *(it->second.begin()->second.begin());
	uint8_t length = wordsDict.at(kana).at(kanji).at(it->first);
	if (location >= it->first && location < it->first + length) {
		QString qkana = kana.c_str();
		QString qkanji = kanji.c_str();
		result = { kana, kanji };
	}
	return result;
}