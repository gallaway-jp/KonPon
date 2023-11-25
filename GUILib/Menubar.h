#pragma once
#include <qmenubar.h>

class Settings;
class TextTree;

class Menubar :
    public QMenuBar
{
    Q_OBJECT
public:
    Menubar(QStyle* style, Settings* settings, const TextTree* textTree);

public:

private:
    QStyle* mStyle;
    Settings* mSettings;
    bool mSettingsDialogOpened = false;
    bool mKeyboardShortcutsDialogOpened = false;

private:
    QAction* createAction(const char* pText, std::function<void()> slot, const QKeySequence& shortcut = QKeySequence::UnknownKey, QIcon* icon = NULL);
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

signals:
    void tokenizeText(const QString& workspace);
};