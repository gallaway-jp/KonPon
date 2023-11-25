#pragma once
#include <qdialog.h>

#include "TextWords.h"

#include <unordered_set>
#include <string>

class QWidget;
template <typename T>
class QList;
class QTextEdit;
class TextEdit;
class QLineEdit;
class QPushButton;

class Settings;
class Wordlist;
class Wordlists;

class TextView :
    public QDialog
{
    using TextDataList = QList<QPair<QStringList*, int64_t>>;
    Q_OBJECT
public:
    TextView(QWidget* parent, uint64_t fileId, Settings* settings, Wordlists& wordlists);
    ~TextView();
    signals:
    void closeDialog();
    //void highlightCustomWordlist(const std::string& name, bool setHighlight = true);
private:
    int64_t mFileId;
    TextWords mTextWords;
    Wordlists& mWordlists;
    TextDataList* m_list; //each element contains pair of lines and offset from beginning of text
    qsizetype m_currentPage;
    QPushButton* m_prevPageButton;
    QPushButton* m_nextPageButton;
    TextEdit* m_textEdit;
    QLineEdit* m_pageEdit;
    std::unordered_set<std::string> mHighlightedCustomWordlists;
    std::pair<std::string, std::string> mCurrentSelectedword;
    int mLastHoveredWordPosition = -1;
    uint8_t mLastHoveredWordLength = 0;
    bool mHighlightUnknownChecked = true;
    bool mHighlightKnownChecked = true;
    int64_t mOffset = 0;
private:
    void highlightCustomWordlist(const std::string& name, bool setHighlight = true);
    void addWidgets();
    void setPageText();
    void changePage();
    void updatePageButtonsState();
    void highlightAllWords();
    void highlightWords(const Wordlist& words, bool setHighlight = true);
    void highlightKnownWords(bool setHighlight = true);
    void highlightUnknownWords(bool setHighlight = true);
    void highlightCustomWords(bool setHighlight = true);

    void handleCursorPositionChanged();
    void handlePageEditEdited(const QString& editedText);
    void handleNextPageButtonClicked();
    void handlePrevPageButtonClicked();
    void handleHighlightCustomButtonClicked();
signals:
    void viewWordClicked(const std::string& kana, const std::string& kanji);
public slots:
    void onSelectedWord(std::pair<std::string, std::string> word);
    void onHoveredWord(std::pair<std::string, std::string> word, const QPoint& globalPosition);
};