#pragma once
#include <QTextEdit>

#include <string>
#include <unordered_set>

#include <String>

class TextWords;

class TextEdit :
    public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget* parent, TextWords& textWords, const int64_t& offset, const qsizetype& currentPage);
private:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event);
    std::pair<std::string, std::string> getWordAtPosition(int wordPosition);

    TextWords& mTextWords;
    const int64_t& mOffset;
    const qsizetype& mCurrentPage;
signals:
    void selectedWord(std::pair<std::string, std::string> word);
    void hoveredWord(std::pair<std::string, std::string> word, const QPoint& globalPosition);
};