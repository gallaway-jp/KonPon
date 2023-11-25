#include "UILib.h"
#include "Settings.h"

#include "Menubar.h"
#include "TextReader.h"
#include "JSONParser.h"

#include <iostream>

#include <QSettings>
#include <QPoint>

#include <qlabel.h>
#include <qcheckbox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <QIntValidator>
#include <QKeyEvent>
#include <QJsonDocument>

UILib::UILib(QString appPath)
    : QMainWindow(nullptr), mAppPath(appPath)
{
    ui.setupUi(this);
    readSettings();
    Settings settings = Settings(mAppPath);

    bool isDarkMode = QApplication::palette().color(QPalette::Window).black() >= 225;
    
    setMenuBar((new Menubar(QApplication::style(), settings.mFile.workspace)));

    //QTextDocument* document = new QTextDocument();
    m_textEdit = new TextReader();
    //m_textEdit = new QTextEdit();
    //m_textEdit->setDocument(document);
    //m_textEdit->setDocumentTitle("Test Title");
    //m_textEdit->setReadOnly(true);

    QFile file("C:\\Users\\Colin\\source\\repos\\UILib\\TestFiles\\input_text.txt");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    JSONParser jsonParser(QStringLiteral("C:\\Users\\Colin\\source\\repos\\UILib\\TestFiles\\input_text.json"));


    QTextStream ReadFile(&file);
    m_list = new QList<QStringList*>();
    QStringList* stringList = new QStringList();
    
    QString* tempString = new QString();
    uint uLineCount = 0;
    while (ReadFile.readLineInto(tempString) != false) {
        uLineCount++;
        if (uLineCount % 50 == 0) {
            m_list->append(stringList);
            stringList = new QStringList();
        }
        stringList->append(*tempString);
    }
    delete tempString;

    if (m_list->length() > 0) {
        m_currentPage = 0;
        setPageText();
    }

    highlightKnownWords();

    addWidgets();

    //m_textEdit->installEventFilter(this);
}

void UILib::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");

    const auto lastPosition = settings.value("position", QPoint()).toPoint();
    if (!lastPosition.isNull()){
        move(lastPosition);
    }
    const auto lastSize = settings.value("size", QPoint()).toSize();
    resize(lastSize);

    settings.endGroup();
}

void UILib::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");

    settings.setValue("position", pos());
    settings.setValue("size", size());

    settings.endGroup();
}


//bool UILib::eventFilter(QObject* obj, QEvent* event)
//{
//    int a = 0;
//
//    if (event->type() == QEvent::KeyPress)
//    {
//        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
//        if (ke->key() == Qt::Key_Left) {
//            if (ke->modifiers() & Qt::ShiftModifier) {
//                // do select
//                return true;
//            }
//            // do cursor move
//            return true;
//        }
//    }
//
//    return QMainWindow::eventFilter(obj, event);
//}

void UILib::addWidgets()
{
    //Add container widget to the application
    QWidget* widget = new QWidget(this);
    UILib::setCentralWidget(widget);

    //Create layouts and container widgets within layouts
    QVBoxLayout* layout = new QVBoxLayout(widget);
    QWidget* bottomWidget = new QWidget();
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);

    layout->addWidget(m_textEdit);
    layout->addWidget(bottomWidget);


    m_pageEdit = new QLineEdit();
    m_pageEdit->setValidator(new QIntValidator(1, m_list->length(), this));
    QLabel* totalPageCountLabel = new QLabel(QString::number(m_list->length()));
    m_prevPageButton = new QPushButton("<");
    m_nextPageButton = new QPushButton(">");

    updatePageButtonsState();
    connect(m_nextPageButton, &QPushButton::released, this, &UILib::handleNextPageButtonClicked);
    connect(m_prevPageButton, &QPushButton::released, this, &UILib::handlePrevPageButtonClicked);

    connect(m_pageEdit, &QLineEdit::textEdited, this, &UILib::handlePageEditEdited);

    connect(m_textEdit, &QTextEdit::cursorPositionChanged, this, &UILib::handleCursorPositionChanged);
    bottomLayout->addWidget(m_pageEdit);
    bottomLayout->addWidget(totalPageCountLabel);
    bottomLayout->addWidget(m_prevPageButton);
    bottomLayout->addWidget(m_nextPageButton);
}

void UILib::handleCursorPositionChanged()
{
    int pos = m_textEdit->textCursor().position();
}

void UILib::setPageText()
{
    QStringList* thisStringList = m_list->at(m_currentPage);
    QString thisString = thisStringList->join('\n');
    m_textEdit->setText(thisString);
}

void UILib::changePage()
{
    setPageText();

    highlightKnownWords();

    updatePageButtonsState();
}

void UILib::handlePageEditEdited(const QString& editedText)
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

void UILib::handleNextPageButtonClicked()
{
    if (m_currentPage >= m_list->length()) {
        return;
    }
    m_currentPage++;
    changePage();
}

void UILib::handlePrevPageButtonClicked()
{
    if (m_currentPage <= 0) {
        return;
    }
    m_currentPage--;
    changePage();
}



void UILib::updatePageButtonsState()
{
    m_pageEdit->setText(QString::number(m_currentPage+1));
    if (m_currentPage+1 >= m_list->length()) {
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

void UILib::highlightKnownWords()
{
    QTextCharFormat fmt;
    fmt.setBackground(Qt::yellow);

    m_textEdit->undo();
    //QTextCursor cursor = m_textEdit->textCursor();
    QTextCursor cursor(m_textEdit->document());
    QString currentText = m_textEdit->toPlainText();
    int begin = 0;
    int end = 5;
    cursor.beginEditBlock();
    while (true) {
        if (cursor.atEnd() || cursor.isNull()) {
            break;
        }
        cursor.setPosition(begin, QTextCursor::MoveAnchor);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);

        begin += 10;
        end += 10;

        if (begin > 3000) {
            int position = cursor.position();
            cursor.movePosition(QTextCursor::End);
            position = cursor.position();
        }
    }
    cursor.endEditBlock();
}


void UILib::closeEvent(QCloseEvent* event)
{
    //if (userReallyWantsToQuit()) {
        writeSettings();
        event->accept();
    //}
   // else {
   //     event->ignore();
    //}
}

UILib::~UILib()
{}
