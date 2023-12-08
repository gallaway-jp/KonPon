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
    QString styleSheetString = qApp->styleSheet();

    QDir resDir = QDir(":/res");
    for (const QFileInfo& fileinfo : resDir.entryInfoList()) {
        QString abspath = fileinfo.absoluteFilePath();
        int a = 0;
    }

    QTranslator translator;
    if (translator.load("", ":/res/translations")) {
        QCoreApplication::installTranslator(&translator);
    }
    
    QCoreApplication::setOrganizationName("Colin Gallaway");
    QCoreApplication::setOrganizationDomain("https://github.com/gallaway-jp/KonPon");
    QCoreApplication::setApplicationName("KonPon");

    mSettings = new Settings();

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
    delete mSettings;
}
