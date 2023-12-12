#include "TextView.h"

#include "CustomCheckboxDialog.h"
#include "Settings.h"
#include "TextEdit.h"
#include "Wordlists.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

TextView::TextView(uint64_t fileId, Settings* settings, Wordlists& wordlists)
	: m_currentPage(0), mFileId(fileId), mWordlists(wordlists), 
    mTextWords(fileId, settings->mFile.workspace.toStdString()), m_settings(settings),
    QDialog(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);
	QFile file(QDir(settings->mFile.workspace + QString("/KonPonData") + QString("/Texts") + QString("/") + QString::number(fileId)).absoluteFilePath("text.txt"));
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QDialog::close();
	}

    QFile nameDataFile(QDir(settings->mFile.workspace + QString("/KonPonData")).absoluteFilePath("TextIds.json"));
    if (nameDataFile.open(QFile::ReadOnly)) {
        QByteArray data = nameDataFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject object = doc.object();
            if (object.contains(QString::number(fileId))
                && object[QString::number(fileId)].isString()) {
                m_textName = object[QString::number(fileId)].toString().toStdString();
            }
        }
        nameDataFile.close();
    }

    QTextStream ReadFile(&file);
    ReadFile.setEncoding(QStringConverter::Encoding::Utf8);
    m_list = new TextDataList();
    QStringList* stringList = new QStringList();

    QString* tempString = new QString();
    uint uLineCount = 0;
    int64_t charCountPrev = 0;
    int64_t charCountCurrent = 0;
    while (ReadFile.readLineInto(tempString) != false) {
        uLineCount++;
        charCountCurrent += tempString->length() + 1; //+1 for newline character
        if (uLineCount % 50 == 0) {
            //0 character offset for first page
            int64_t offset = 0;
            if (charCountPrev != 0) {
                offset = charCountPrev;
            }
            charCountPrev = charCountCurrent;
            m_list->append({ stringList, offset });
            stringList = new QStringList();
        }
        stringList->append(*tempString);
    }
    if (!stringList->isEmpty()) {
        m_list->append({ stringList, charCountPrev });
    }
    delete tempString;
    file.close();

    if (m_list->length() <= 0) {
        QDialog::close();
    }
    else {
        m_textEdit = new TextEdit(nullptr, mTextWords, mOffset, m_currentPage);
        connect(m_textEdit, &TextEdit::selectedWord, this, &TextView::onSelectedWord);
        connect(m_textEdit, &TextEdit::hoveredWord, this, &TextView::onHoveredWord);
        setPageText();
        highlightAllWords();
        addWidgets();
    }

    setWindowTitle(tr("Text") + " - " + m_textName.c_str());
}
#include <QScrollArea>
#include <QCheckbox>
#include <QListWidget>
void TextView::addWidgets()
{
    //Create layouts and container widgets within layouts
    QVBoxLayout* layout = new QVBoxLayout(this);
    QWidget* bottomWidget = new QWidget();
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);
    layout->addWidget(m_textEdit);
    
    m_highlightUnknownCheckbox = new QCheckBox(tr("Highlight Unknown Words"));
    m_highlightUnknownCheckbox->setChecked(true);
    m_highlightKnownCheckbox = new QCheckBox(tr("Highlight Known Words"));
    m_highlightKnownCheckbox->setChecked(true);
    m_highlightCustomButton = new QPushButton(tr("Highlight Words From Custom List"));
    QHBoxLayout* checkboxLayout = new QHBoxLayout();
    checkboxLayout->addWidget(m_highlightUnknownCheckbox);
    checkboxLayout->addWidget(m_highlightKnownCheckbox);
    checkboxLayout->addWidget(m_highlightCustomButton);
    layout->addLayout(checkboxLayout);

    layout->addWidget(bottomWidget);


    m_pageEdit = new QLineEdit();
    m_pageEdit->setValidator(new QIntValidator(1, m_list->length(), this));
    QLabel* totalPageCountLabel = new QLabel(QString::number(m_list->length()));
    m_prevPageButton = new QPushButton("<");
    m_nextPageButton = new QPushButton(">");

    updatePageButtonsState();

    connect(m_nextPageButton, &QPushButton::released, this, &TextView::handleNextPageButtonClicked);
    connect(m_prevPageButton, &QPushButton::released, this, &TextView::handlePrevPageButtonClicked);
    connect(m_pageEdit, &QLineEdit::textEdited, this, &TextView::handlePageEditEdited);
    connect(m_textEdit, &QTextEdit::cursorPositionChanged, this, &TextView::handleCursorPositionChanged);
    connect(m_highlightCustomButton, &QPushButton::clicked, this, &TextView::handleHighlightCustomButtonClicked);
    connect(m_highlightKnownCheckbox, &QCheckBox::toggled, this, &TextView::highlightKnownWords);
    connect(m_highlightUnknownCheckbox, &QCheckBox::toggled, this, &TextView::highlightUnknownWords);
    

    bottomLayout->addWidget(m_pageEdit);
    bottomLayout->addWidget(totalPageCountLabel);
    bottomLayout->addWidget(m_prevPageButton);
    bottomLayout->addWidget(m_nextPageButton);
}

void TextView::highlightCustomWordlist(const std::string& name, bool setHighlight)
{
    highlightWords(mWordlists.getCustomWordlist(name), setHighlight);
    if (setHighlight) {
        mHighlightedCustomWordlists.insert(name);
        return;
    }
    mHighlightedCustomWordlists.erase(name);
}

void TextView::setPageText()
{
    QStringList* thisStringList = m_list->at(m_currentPage).first;
    QString thisString = thisStringList->join('\n');
    m_textEdit->setText(thisString);
}

void TextView::changePage()
{
    mOffset = m_list->at(m_currentPage).second;
    setPageText();
    highlightKnownWords(mHighlightKnownChecked);
    highlightUnknownWords(mHighlightUnknownChecked);
    for (const auto& customWordlistName : mHighlightedCustomWordlists) {
        highlightWords(mWordlists.getCustomWordlist(customWordlistName));
    }
    updatePageButtonsState();
}

void TextView::handlePageEditEdited(const QString& editedText)
{
    qsizetype page = editedText.toLongLong();
    qsizetype pageCount = m_list->length();
    if (page > 0) {
        if (page <= pageCount) {
            m_currentPage = page - 1;
        }
        else {
            // If user enters page number bigger than page count, then move to final page
            m_currentPage = m_list->length() - 1;
        }
        changePage();
    }
}

void TextView::handleNextPageButtonClicked()
{
    if (m_currentPage >= m_list->length()) {
        return;
    }
    m_currentPage++;
    changePage();
}

void TextView::handlePrevPageButtonClicked()
{
    if (m_currentPage <= 0) {
        return;
    }
    m_currentPage--;
    changePage();
}

void TextView::handleHighlightCustomButtonClicked()
{
    CustomCheckboxDialog dialog(this, mWordlists.getCustomWordlistNames(), mHighlightedCustomWordlists);
    connect(&dialog, &CustomCheckboxDialog::toggleCustomCheckbox, this, &TextView::highlightCustomWordlist);
    dialog.exec();
}

void TextView::updatePageButtonsState()
{
    m_pageEdit->setText(QString::number(m_currentPage + 1));
    if (m_currentPage + 1 >= m_list->length()) {
        m_nextPageButton->setEnabled(false);
    }
    else {
        m_nextPageButton->setEnabled(true);
    }
    if (m_currentPage - 1 < 0) {
        m_prevPageButton->setEnabled(false);
    }
    else {
        m_prevPageButton->setEnabled(true);
    }
}

void TextView::highlightAllWords()
{
    highlightUnknownWords();
    highlightKnownWords();
    highlightCustomWords();
}

void TextView::highlightWords(const Wordlist& words, bool setHighlight)
{
    QTextCharFormat fmt;
    if (setHighlight) {
        auto color = words.getColor();
        if (color != WordListInfo::Color::NOCOLOR) {
            fmt.setBackground(static_cast<Qt::GlobalColor>(
                m_settings->ui.isDarkTheme() ? WordListInfo::Colors.at(color).second
                : WordListInfo::Colors.at(color).first));
        }
    }

    QTextCursor cursor(m_textEdit->document());
    QString currentText = m_textEdit->toPlainText();

    const auto& locations = mTextWords.getLocations();
    auto itBegin = locations.lower_bound(mOffset);
    auto itEnd = locations.end();
    if (m_currentPage < m_list->count() - 1) {
        // If not last page, set end location as beginning of next page's offset
        itEnd = mTextWords.getLocations().lower_bound(m_list->at(m_currentPage+1).second);
    }

    cursor.beginEditBlock();
    for(auto it = itBegin; it != itEnd; ++it){
        const auto& location = it->first;
        const auto& kanaDict = it->second;
        for (const auto& [kana, kanjiSet] : kanaDict) {
            for (const auto& kanji : kanjiSet) {
                if (!words.contains(kana, kanji)) {
                    continue;
                }
                const auto& wordsDict = mTextWords.getWords();
                uint8_t length = wordsDict.at(kana).at(kanji).at(location);
                int64_t position = location - mOffset + ((m_currentPage==0)?0:1);
                cursor.setPosition(position, QTextCursor::MoveAnchor);
                cursor.setPosition(position + length, QTextCursor::KeepAnchor);
                cursor.setCharFormat(fmt);
                break;
            }
            break;
        }
    }
    cursor.endEditBlock();
}

void TextView::highlightKnownWords(bool setHighlight)
{
    highlightWords(mWordlists.getKnownWordlist(), setHighlight);
    mHighlightKnownChecked = setHighlight;
}

void TextView::highlightUnknownWords(bool setHighlight)
{
    highlightWords(mWordlists.getUnknownWordlist(), setHighlight);
    mHighlightUnknownChecked = setHighlight;
}

void TextView::highlightCustomWords(bool setHighlight)
{
    for (const auto& customWordlistName : mWordlists.getCustomWordlistNames()) {
        highlightWords(mWordlists.getCustomWordlist(customWordlistName), setHighlight);
        mHighlightedCustomWordlists.insert(customWordlistName);
    }
}

void TextView::handleCursorPositionChanged()
{
    int pos = m_textEdit->textCursor().position();
}

void TextView::onSelectedWord(std::pair<std::string, std::string> word)
{
    emit viewWordClicked(word.first, word.second);
}

#include <QToolTip>
void TextView::onHoveredWord(std::pair<std::string, std::string> word, const QPoint& globalPosition)
{
    QString wordlistName;
    if (mWordlists.getKnownWordlist().contains(word.first, word.second)) {
        wordlistName = tr("Known Wordlist");
    }
    else if (mWordlists.getUnknownWordlist().contains(word.first, word.second)) {
        wordlistName = tr("Unknown Wordlist");
    }
    else {
        for (const auto& customName : mWordlists.getCustomWordlistNames()) {
            if (mWordlists.getCustomWordlist(customName).contains(word.first, word.second)) {
                wordlistName = customName.c_str();
                break;
            }
        }
    }
    QString text = !word.first.empty() && !word.second.empty()
        ? tr("Kana: ").append(QString("%1\n").arg(word.first.c_str())).append(tr("Kanji: ")).append(word.second.c_str())
        .append("\n").append(tr("Wordlist: ")).append(wordlistName)
        : "";
    QToolTip::showText(globalPosition, text);
}

void TextView::onRetranslateUI()
{
    setWindowTitle(QCoreApplication::translate("TextView", "Text") + " - " + m_textName.c_str());

    m_highlightUnknownCheckbox->setText(QCoreApplication::translate("TextView", "Highlight Unknown Words"));
    m_highlightKnownCheckbox->setText(QCoreApplication::translate("TextView", "Highlight Known Words"));
    m_highlightCustomButton->setText(QCoreApplication::translate("TextView", "Highlight Words From Custom List"));
}

TextView::~TextView()
{
    emit closeDialog();
}