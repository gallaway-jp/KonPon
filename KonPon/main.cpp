#include "KonPon.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KonPon w;
    w.show();
    return a.exec();
}
