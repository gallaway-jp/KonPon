#include "Menubar.h"

#include "Settings.h"
#include "SettingsDialog.h"
#include "ShortcutsDialog.h"
#include "TextTree.h"

#include <QAction>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>

#define GET_STANDARD_ICON(icon) /*QApplication::*/mStyle->standardIcon(QStyle::StandardPixmap::##icon)
#define PASS_METHOD(method) [&](){method();}

Menubar::Menubar(QStyle* style, Settings* settings, const TextTree* textTree)
    : mStyle(style), mSettings(settings)
{
    addMenus();
    onRetranslateUI();
    connect(this, &Menubar::tokenizeText, textTree, &TextTree::onTokenizeText);
    connect(this, &Menubar::restranslateUI, this, &Menubar::onRetranslateUI);
}

QAction* Menubar::createAction(const char* pText, std::function<void()> slot, const QKeySequence& shortcut, QIcon* icon)
{
    QAction* retAction = new QAction(pText, this);
    if (icon) {
        retAction->setIcon(*icon);
    }

    if (slot) {
        connect(retAction, &QAction::triggered, this, slot);
    }

    if (shortcut != QKeySequence::UnknownKey) {
        retAction->setShortcut(shortcut);
    }
    else {
        retAction->setDisabled(true);
    }

    return retAction;
}

QList<QAction*>* Menubar::createFileActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;

    m_importTextAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Import Text"),
        PASS_METHOD(onImportText), Qt::CTRL | Qt::Key_O, &(tmpIcon = GET_STANDARD_ICON(SP_FileIcon)));
    retActions->append(m_importTextAction);
    m_importAudioAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Import Audio"),
        PASS_METHOD(onImportAudio), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_A*/, &(tmpIcon = GET_STANDARD_ICON(SP_MediaVolume)));
    retActions->append(m_importAudioAction);
    m_importTextAudioAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Import Text && Audio"),
        PASS_METHOD(onImportTextAudio), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_B*/, &(tmpIcon = GET_STANDARD_ICON(SP_DriveCDIcon)));
    retActions->append(m_importTextAudioAction);
    m_viewWordListsAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&View Word Lists"),
        PASS_METHOD(onViewWordLists), Qt::CTRL | Qt::Key_W, &(tmpIcon = GET_STANDARD_ICON(SP_FileDialogContentsView)));
    retActions->append(m_viewWordListsAction);
    m_exitAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Exit"),
        PASS_METHOD(onExit), Qt::CTRL | Qt::Key_Q, &(tmpIcon = GET_STANDARD_ICON(SP_DialogCloseButton)));
    retActions->append(m_exitAction);
    return retActions;
}

QList<QAction*>* Menubar::createToolsActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;
    m_syncAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Sync"),
        PASS_METHOD(onSync), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_Y*/, &(tmpIcon = GET_STANDARD_ICON(SP_DriveNetIcon)));
    retActions->append(m_syncAction);
    m_settingsAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Settings"),
        PASS_METHOD(onSettings), Qt::CTRL | Qt::Key_T, &(tmpIcon = GET_STANDARD_ICON(SP_ComputerIcon)));
    retActions->append(m_settingsAction);
    return retActions;
}

QList<QAction*>* Menubar::createHelpActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;
    m_manualAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Manual"),
        PASS_METHOD(onManual), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_M*/, &(tmpIcon = GET_STANDARD_ICON(SP_DialogHelpButton)));
    retActions->append(m_manualAction);
    m_releaseNotesAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Release Notes"),
        PASS_METHOD(onReleaseNotes), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_N*/, &(tmpIcon = GET_STANDARD_ICON(SP_DialogApplyButton)));
    retActions->append(m_releaseNotesAction);
    m_videoTutorialsAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Video Tutorials"),
        PASS_METHOD(onVideoTutorials), Qt::CTRL | Qt::Key_V, &(tmpIcon = GET_STANDARD_ICON(SP_MediaPlay)));
    retActions->append(m_videoTutorialsAction);
    m_keyboardShortcutsAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Keyboard Shortcuts"),
        PASS_METHOD(onKeyboardShortcuts), Qt::CTRL | Qt::Key_K, &(tmpIcon = GET_STANDARD_ICON(SP_MediaSkipForward)));
    retActions->append(m_keyboardShortcutsAction);
    m_viewGithubAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&View Github"),
        PASS_METHOD(onViewGithub), Qt::CTRL | Qt::Key_H, &(tmpIcon = GET_STANDARD_ICON(SP_MessageBoxInformation)));
    retActions->append(m_viewGithubAction);
    m_checkUpdatesAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&Check for Updates"),
        PASS_METHOD(onCheckUpdates), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_U*/, &(tmpIcon = GET_STANDARD_ICON(SP_ArrowUp)));
    retActions->append(m_checkUpdatesAction);
    m_aboutAction = createAction(QT_TRANSLATE_NOOP("Menubar", "&About"),
        PASS_METHOD(onAbout), Qt::CTRL | Qt::Key_I, &(tmpIcon = GET_STANDARD_ICON(SP_FileDialogInfoView)));
    retActions->append(m_aboutAction);
    return retActions;
}

void Menubar::addMenus()
{
    QList<QAction*>* fileActions = createFileActions();
    m_fileMenu = addMenu(tr("&File"));
    m_fileMenu->addActions(*fileActions);
    delete fileActions;

    QList<QAction*>* toolsActions = createToolsActions();
    m_toolsMenu = addMenu(tr("&Tools"));
    m_toolsMenu->addActions(*toolsActions);
    delete toolsActions;

    QList<QAction*>* helpActions = createHelpActions();
    m_helpMenu = addMenu(tr("&Help"));
    m_helpMenu->addActions(*helpActions);
    delete helpActions;
}

void Menubar::onImportText()
{
    emit tokenizeText(mSettings->mFile.workspace);
}

void Menubar::onImportAudio()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Audio"), "", tr("Audio Files (*.mp3)"));
}

void Menubar::onImportTextAudio()
{
    //QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Audio"), "", tr("Audio Files (*.mp3)"));
}

void Menubar::onViewWordLists()
{
    emit viewWordLists();
}

void Menubar::onExit()
{
    //QApplication::quit();
}

void Menubar::onSync()
{
    ;
}

void Menubar::onSettings()
{
    if (m_settingsDialog == nullptr) {
        mSettingsDialogOpened = true;
        m_settingsDialog = new SettingsDialog(mSettings);
        connect(m_settingsDialog, &SettingsDialog::retranslateUI, this, &Menubar::restranslateUI);
        connect(m_settingsDialog, &SettingsDialog::closeDialog, this, [this]() { m_settingsDialog = nullptr; });
    }
    m_settingsDialog->show();
    m_settingsDialog->raise();
    m_settingsDialog->activateWindow();
}

void Menubar::onManual()
{
    ;
}

void Menubar::onReleaseNotes()
{
    ;
}

void Menubar::onVideoTutorials()
{
    QDesktopServices::openUrl(QUrl("https://github.com/gallaway-jp/KonPon/wiki/Tutorials"));
}

void Menubar::onKeyboardShortcuts()
{
    if (m_shortcutsDialog == nullptr) {
        m_shortcutsDialog = new ShortcutsDialog();
        connect(this, &Menubar::restranslateUI, m_shortcutsDialog, &ShortcutsDialog::onRetranslateUI);
        connect(m_shortcutsDialog, &ShortcutsDialog::closeDialog, this, [this]() { m_shortcutsDialog = nullptr; });
    }
    m_shortcutsDialog->show();
    m_shortcutsDialog->raise();
    m_shortcutsDialog->activateWindow();
}

void Menubar::onViewGithub()
{
    QDesktopServices::openUrl(QUrl("https://github.com/gallaway-jp/KonPon"));
}

void Menubar::onCheckUpdates()
{
    ;
}

void Menubar::onAbout()
{
    QMessageBox::about(this, tr("About"), tr("Made by Colin Gallaway."));
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void Menubar::onRetranslateUI()
{
    m_fileMenu->setTitle(QCoreApplication::translate("Menubar", "&File"));
    m_importTextAction->setText(QCoreApplication::translate("Menubar", "&Import Text"));
    m_importAudioAction->setText(QCoreApplication::translate("Menubar", "&Import Audio"));
    m_importTextAudioAction->setText(QCoreApplication::translate("Menubar", "&Import Text && Audio"));
    m_viewWordListsAction->setText(QCoreApplication::translate("Menubar", "&View Word Lists"));
    m_exitAction->setText(QCoreApplication::translate("Menubar", "&Exit"));

    m_toolsMenu->setTitle(QCoreApplication::translate("Menubar", "&Tools"));
    m_syncAction->setText(QCoreApplication::translate("Menubar", "&Sync"));
    m_settingsAction->setText(QCoreApplication::translate("Menubar", "&Settings"));

    m_helpMenu->setTitle(QCoreApplication::translate("Menubar", "&Help"));
    m_manualAction->setText(QCoreApplication::translate("Menubar", "&Manual"));
    m_releaseNotesAction->setText(QCoreApplication::translate("Menubar", "&Release Notes"));
    m_videoTutorialsAction->setText(QCoreApplication::translate("Menubar", "&Video Tutorials"));
    m_keyboardShortcutsAction->setText(QCoreApplication::translate("Menubar", "&Keyboard Shortcuts"));
    m_viewGithubAction->setText(QCoreApplication::translate("Menubar", "&View Github"));
    m_checkUpdatesAction->setText(QCoreApplication::translate("Menubar", "&Check for Updates"));
    m_aboutAction->setText(QCoreApplication::translate("Menubar", "&About"));
}