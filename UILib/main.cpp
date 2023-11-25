#include "UILib.h"
#include "Application.h"
#include "Settings.h"

#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");

    QCoreApplication::setOrganizationName("Colin Gallaway");
    QCoreApplication::setOrganizationDomain("https://github.com/gallaway-jp/KonPon");
    QCoreApplication::setApplicationName("KonPon");

    UILib w = UILib(QCoreApplication::applicationDirPath());

    w.show();
    return a.exec();
}