#include "MainWindow.h"

#include "Wordlists.h"
#include "Settings.h"
#include "Menubar.h"
#include "CentralWidget.h"

#include <QCoreApplication>
#include <QApplication>

inline MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QApplication::setStyle("fusion");
    Q_INIT_RESOURCE(Resource);
    
    QCoreApplication::setOrganizationName("Colin Gallaway");
    QCoreApplication::setOrganizationDomain("https://github.com/gallaway-jp/KonPon");
    QCoreApplication::setApplicationName("KonPon");

    mSettings = new Settings();

    CentralWidget *centralWidget = new CentralWidget(this, mSettings);

    Menubar* menubar = new Menubar(mSettings, centralWidget->mTextTree);
    connect(menubar, &Menubar::restranslateUI, centralWidget, &CentralWidget::retranslateUI);
    connect(menubar, &Menubar::viewWordLists, centralWidget, &CentralWidget::viewWordLists);
    setMenuBar(menubar);
    setCentralWidget(centralWidget);
}

inline void MainWindow::closeEvent(QCloseEvent* event)
{
    QApplication::closeAllWindows();
}

inline MainWindow::~MainWindow()
{
    delete mSettings;
}
