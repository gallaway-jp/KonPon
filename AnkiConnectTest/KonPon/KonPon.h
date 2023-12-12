#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_KonPon.h"
#include "MainWindow.cpp"

class KonPon : public MainWindow
{
    //Q_OBJECT

public:
    KonPon(QWidget* parent = nullptr);
    ~KonPon();

private:
    Ui::KonPonClass ui;
};
