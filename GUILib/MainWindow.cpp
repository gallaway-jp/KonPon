#include "MainWindow.h"

#include "Wordlists.h"
#include "Settings.h"
#include "Menubar.h"
#include "CentralWidget.h"

#include <QCoreApplication>
#include <QApplication>
#include <QStyleHints>
#include <QTranslator>
#include <QDir>

inline MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QApplication::setStyle("fusion");

    QDir resDir = QDir(":/res");
    for (const QFileInfo& fileinfo : resDir.entryInfoList()) {
        QString abspath = fileinfo.absoluteFilePath();
        int a = 0;
    }

    QTranslator translator;
    //if (translator.load("Translation_ja.qm", ":/res/translations")) {
    if (translator.load("", ":/res/translations")) {
        QCoreApplication::installTranslator(&translator);
    }
    

    QCoreApplication::setOrganizationName("Colin Gallaway");
    QCoreApplication::setOrganizationDomain("https://github.com/gallaway-jp/KonPon");
    QCoreApplication::setApplicationName("KonPon");

    _mIsDarkMode = QApplication::palette().color(QPalette::Window).black() >= 225;
    
    QGuiApplication::styleHints()->colorScheme();
    //this->style()->styleHint(QStyle::colors)

    QString appDirPath = QCoreApplication::applicationDirPath();
    mSettings = new Settings(appDirPath);
    //mWordlists = new Wordlists(mSettings->mFile.workspace.toStdString());

    CentralWidget *centralWidget = new CentralWidget(this, mSettings);

    setMenuBar((new Menubar(QApplication::style(), mSettings, centralWidget->mTextTree)));
    setCentralWidget(centralWidget);
}

inline void MainWindow::changeEvent(QEvent* event)
{
    //if (event != 0) {
    //    switch (event->type()) {
    //        // this event is send if a translator is loaded
    //    case QEvent::LanguageChange:
    //    {
    //        QTranslator translator = QTranslator();
    //        translator.translate()

    //        //retransl
    //        //ui.retranslateUi(this);
    //        break;
    //    }
    //        // this event is send, if the system, language changes
    //    case QEvent::LocaleChange:
    //    {
    //        //QString locale = QLocale::system().name();
    //        //locale.truncate(locale.lastIndexOf('_'));
    //        //loadLanguage(locale);
    //    }
    //    break;
    //    }
    //}
    QMainWindow::changeEvent(event);
}

inline MainWindow::~MainWindow()
{
    //delete mWordlists;
    delete mSettings;
}
