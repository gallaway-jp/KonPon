#pragma once
#include <qdialog.h>
class ShortcutsDialog :
    public QDialog
{
    Q_OBJECT
public:
    ShortcutsDialog(bool& keyboardShortcutsDialogOpened);

private:
    bool& mKeyboardShortcutsDialogOpened;

private:
    void reject();
};

