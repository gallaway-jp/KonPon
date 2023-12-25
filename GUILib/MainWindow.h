#pragma once

#include "guilib_global.h"
#include <QtWidgets/QMainWindow>

class Settings;
class Wordlists;

class GUILIB_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent* event);
signals:
    void shutdownApp();
private:
    Settings* mSettings;
};
