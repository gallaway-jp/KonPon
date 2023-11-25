#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UILib.h"
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <QString>

class UILib : public QMainWindow
{
    Q_OBJECT

public:
    UILib(QString appPath);
    ~UILib();

private:
    QString mAppPath;

    Ui::UILibClass ui;
    QList<QStringList*>* m_list;
    qsizetype m_currentPage;
    QPushButton* m_prevPageButton;
    QPushButton* m_nextPageButton;

    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent* event);

    void addWidgets();
    void handleCursorPositionChanged();
    void handlePageEditEdited(const QString& editedText);
    void handleNextPageButtonClicked();
    void handlePrevPageButtonClicked();

    QTextEdit* m_textEdit;
    QLineEdit* m_pageEdit;

    void setPageText();
    void changePage();
    void updatePageButtonsState();
    void highlightKnownWords();

    //bool eventFilter(QObject* obj, QEvent* event);
};
