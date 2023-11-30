#include "Menubar.h"

#include "SettingsDialog.h"
#include "ShortcutsDialog.h"
#include "Settings.h"
#include "TextTree.h"

#include <QHBoxLayout>

#include <QMenu.h>
#include <QAction.h>
#include <QStyle.h>
#include <QIcon.h>

#include <QFileDialog.h>
#include <QMessageBox.h>
#include <QDesktopServices.h>

#define GET_STANDARD_ICON(icon) /*QApplication::*/mStyle->standardIcon(QStyle::StandardPixmap::##icon)
#define PASS_METHOD(method) [&](){method();}

Menubar::Menubar(QStyle* style, Settings* settings, const TextTree* textTree)
    : mStyle(style), mSettings(settings)
{
    addMenus();
    connect(this, &Menubar::tokenizeText, textTree, &TextTree::onTokenizeText);
}

QAction* Menubar::createAction(const char* pText, std::function<void()> slot, const QKeySequence& shortcut, QIcon* icon)
{
    QAction* retAction = new QAction((tr(pText)), this);
    if (icon) {
        retAction->setIcon(*icon);
    }

    retAction->setStatusTip(tr("Parses the input text and adds text and words to app"));
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
    retActions->append(createAction(QT_TR_NOOP("&Import Text"),
        PASS_METHOD(onImportText), Qt::CTRL | Qt::Key_O, &(tmpIcon = GET_STANDARD_ICON(SP_FileIcon))));
    retActions->append(createAction(QT_TR_NOOP("&Import Audio"),
        PASS_METHOD(onImportAudio), QKeySequence::UnknownKey/*Qt::CTRL | Qt::Key_A*/, &(tmpIcon = GET_STANDARD_ICON(SP_MediaVolume))));
    retActions->append(createAction(QT_TR_NOOP("&Import Text & Audio"),
        PASS_METHOD(onImportTextAudio), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DriveCDIcon))));
    retActions->append(createAction(QT_TR_NOOP("&View Word Lists"),
        PASS_METHOD(onViewWordLists), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_FileDialogContentsView))));
    retActions->append(createAction(QT_TR_NOOP("&Exit"),
        PASS_METHOD(onExit), Qt::Key_Q, &(tmpIcon = GET_STANDARD_ICON(SP_DialogCloseButton))));
    return retActions;
}

QList<QAction*>* Menubar::createToolsActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;
    retActions->append(createAction(QT_TR_NOOP("&Sync"),
        PASS_METHOD(onSync), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DriveNetIcon))));
    retActions->append(createAction(QT_TR_NOOP("&Settings"),
        PASS_METHOD(onSettings), Qt::CTRL | Qt::Key_T, &(tmpIcon = GET_STANDARD_ICON(SP_ComputerIcon))));
    return retActions;
}

QList<QAction*>* Menubar::createHelpActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;
    retActions->append(createAction(QT_TR_NOOP("&Manual"),
        PASS_METHOD(onManual), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DialogHelpButton))));
    retActions->append(createAction(QT_TR_NOOP("&Release Notes"),
        PASS_METHOD(onReleaseNotes), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DialogApplyButton))));
    retActions->append(createAction(QT_TR_NOOP("&Video Tutorials"),
        PASS_METHOD(onVideoTutorials), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_MediaPlay))));
    retActions->append(createAction(QT_TR_NOOP("&Keyboard Shortcuts"),
        PASS_METHOD(onKeyboardShortcuts), Qt::CTRL | Qt::Key_K, &(tmpIcon = GET_STANDARD_ICON(SP_MediaSkipForward))));
    retActions->append(createAction(QT_TR_NOOP("&View Github"),
        PASS_METHOD(onViewGithub), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_MessageBoxInformation))));
    retActions->append(createAction(QT_TR_NOOP("&Check for Updates"),
        PASS_METHOD(onCheckUpdates), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_ArrowUp))));
    retActions->append(createAction(QT_TR_NOOP("&About"),
        PASS_METHOD(onAbout), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_FileDialogInfoView))));
    return retActions;
}

void Menubar::addMenus()
{
    QList<QAction*>* fileActions = createFileActions();
    addMenu(tr("&File"))->addActions(*fileActions);
    delete fileActions;

    QList<QAction*>* toolsActions = createToolsActions();
    addMenu(tr("&Tools"))->addActions(*toolsActions);
    delete toolsActions;

    QList<QAction*>* helpActions = createHelpActions();
    addMenu(tr("&Help"))->addActions(*helpActions);
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
    ;
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
    if (mSettingsDialogOpened == false) {
        mSettingsDialogOpened = true;
        SettingsDialog* settingsDialog = new SettingsDialog(mSettings, mSettingsDialogOpened);
        settingsDialog->show();
    }
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
    QDesktopServices::openUrl(QUrl("https://google.com/"));
}

void Menubar::onKeyboardShortcuts()
{
    if (mKeyboardShortcutsDialogOpened == false) {
        mKeyboardShortcutsDialogOpened = true;
        ShortcutsDialog* keyboardShortcutsDialog = new ShortcutsDialog(mKeyboardShortcutsDialogOpened);
        keyboardShortcutsDialog->show();
    }
}

void Menubar::onViewGithub()
{
    QDesktopServices::openUrl(QUrl("https://google.com/"));
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



