#pragma once
#include <QMenuBar>

class Settings;
class SettingsDialog;
class ShortcutsDialog;
class TextTree;

class QMenu;

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
    SettingsDialog* m_settingsDialog = nullptr;
    ShortcutsDialog* m_shortcutsDialog = nullptr;

private:
    QAction* createAction(const char* pText, std::function<void()> slot, const QKeySequence& shortcut = QKeySequence::UnknownKey, QIcon* icon = NULL);
    QList<QAction*>* createFileActions();
    QList<QAction*>* createToolsActions();
    QList<QAction*>* createHelpActions();
    void addMenus();
private:
    QMenu* m_fileMenu = nullptr;
    QAction* m_importTextAction = nullptr;
    QAction* m_importAudioAction = nullptr;
    QAction* m_importTextAudioAction = nullptr;
    QAction* m_viewWordListsAction = nullptr;
    QAction* m_exitAction = nullptr;

    QMenu* m_toolsMenu = nullptr;
    QAction* m_syncAction = nullptr;
    QAction* m_settingsAction = nullptr;

    QMenu* m_helpMenu = nullptr;
    QAction* m_manualAction = nullptr;
    QAction* m_releaseNotesAction = nullptr;
    QAction* m_videoTutorialsAction = nullptr;
    QAction* m_keyboardShortcutsAction = nullptr;
    QAction* m_viewGithubAction = nullptr;
    QAction* m_checkUpdatesAction = nullptr;
    QAction* m_aboutAction = nullptr;
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
    void restranslateUI();
private slots:
    void onRetranslateUI();
};