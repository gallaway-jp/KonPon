#pragma once
#include <qmenubar.h>
class Menubar :
    public QMenuBar
{
    Q_OBJECT
public:
    Menubar(QStyle *style, QString workspace);
    ~Menubar();

public:

private:
    QStyle *mStyle;
    QString mWorkspace;
    bool mSettingsDialogOpened = false;
    bool mKeyboardShortcutsDialogOpened = false;

private:
    QAction* createAction(const char* pText, std::function<void()> slot, const QKeySequence &shortcut = QKeySequence::UnknownKey,QIcon* icon = NULL);
    QList<QAction*>* createFileActions();
    QList<QAction*>* createToolsActions();
    QList<QAction*>* createHelpActions();
    void addMenus();

private:
    void onImportText();
    void onImportAudio();
    void onImportTextAudio();
    void onViewWordLists();
    void onExit();
    void onSync();
    void onSettings();
    void onManual();
    void onReleaseNotes();
    void onVideoTutorials();
    void onKeyboardShortcuts();
    void onViewGithub();
    void onCheckUpdates();
    void onAbout();
};

