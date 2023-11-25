#pragma once

#include "DialogManager.h"

#include <unordered_map>

#include <QWidget>

class TextTree;
class Settings;
class Wordlists;
class DialogManager;

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(QWidget* parent, Settings* settings); 
    ~CentralWidget();
public:
    TextTree* mTextTree;
private:
    Wordlists* mWordlists;
    DialogManager* mDlgMgr = nullptr;
};
