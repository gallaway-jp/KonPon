#include "Menubar.h"

#include "Strings.h"

#include "SettingsDialog.h"
#include "ShortcutsDialog.h"

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

Menubar::Menubar(QStyle* style, QString workspace)
    : mStyle(style), mWorkspace(workspace)
{
	addMenus();
}

QAction* Menubar::createAction(const char *pText, std::function<void()> slot, const QKeySequence &shortcut, QIcon* icon)
{
    QAction* retAction = new QAction((tr(pText)), this);
    if (icon) {
        retAction->setIcon(*icon);
    }

    retAction->setStatusTip(tr(MENU_FILE_IMPORT_TEXT_TIP));
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
    QList<QAction*> *retActions = new QList<QAction*>();
    QIcon tmpIcon;
    retActions->append(createAction(MENU_FILE_IMPORT_TEXT, 
        PASS_METHOD(onImportText), Qt::CTRL | Qt::Key_O, &(tmpIcon = GET_STANDARD_ICON(SP_FileIcon))));
    retActions->append(createAction(MENU_FILE_IMPORT_AUDIO,
        PASS_METHOD(onImportAudio), Qt::CTRL | Qt::Key_A,&(tmpIcon = GET_STANDARD_ICON(SP_MediaVolume))));
    retActions->append(createAction(MENU_FILE_IMPORT_TEXT_AUDIO,
        PASS_METHOD(onImportTextAudio), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DriveCDIcon))));
    retActions->append(createAction(MENU_FILE_VIEW_WORD_LISTS,
        PASS_METHOD(onViewWordLists), QKeySequence::UnknownKey , &(tmpIcon = GET_STANDARD_ICON(SP_FileDialogContentsView))));
    retActions->append(createAction(MENU_FILE_EXIT,
        PASS_METHOD(onExit), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DialogCloseButton))));
    return retActions;
}

QList<QAction*>* Menubar::createToolsActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;
    retActions->append(createAction(MENU_TOOLS_SYNC,
        PASS_METHOD(onSync), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DriveNetIcon))));
    retActions->append(createAction(MENU_TOOLS_SETTINGS,
        PASS_METHOD(onSettings), Qt::CTRL | Qt::Key_T, &(tmpIcon = GET_STANDARD_ICON(SP_ComputerIcon))));
    return retActions;
}

QList<QAction*>* Menubar::createHelpActions()
{
    QList<QAction*>* retActions = new QList<QAction*>();
    QIcon tmpIcon;
    retActions->append(createAction(MENU_HELP_MANUAL,
        PASS_METHOD(onManual), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DialogHelpButton))));
    retActions->append(createAction(MENU_HELP_RELEASE_NOTES,
        PASS_METHOD(onReleaseNotes), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_DialogApplyButton))));
    retActions->append(createAction(MENU_HELP_VIDEO_TUTORIALS,
        PASS_METHOD(onVideoTutorials), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_MediaPlay))));
    retActions->append(createAction(MENU_HELP_KEYBOARD_SHORTCUTS,
        PASS_METHOD(onKeyboardShortcuts), Qt::CTRL | Qt::Key_K, &(tmpIcon = GET_STANDARD_ICON(SP_MediaSkipForward))));
    retActions->append(createAction(MENU_HELP_VIEW_GITHUB,
        PASS_METHOD(onViewGithub), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_MessageBoxInformation))));
    retActions->append(createAction(MENU_HELP_CHECK_UPDATES,
        PASS_METHOD(onCheckUpdates), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_ArrowUp))));
    retActions->append(createAction(MENU_HELP_ABOUT,
        PASS_METHOD(onAbout), QKeySequence::UnknownKey, &(tmpIcon = GET_STANDARD_ICON(SP_FileDialogInfoView))));
    return retActions;
}

#define ADD_MENU(menuVar, MENUSTRING)                               \
    QList<QAction*> *menuVar##Actions = create##menuVar##Actions(); \
    addMenu(tr(MENU_##MENUSTRING))->addActions(*menuVar##Actions);  \
    delete menuVar##Actions;

void Menubar::addMenus()
{
    ADD_MENU(File, FILE);
    ADD_MENU(Tools, TOOLS);
    ADD_MENU(Help, HELP);
}

#define BUILD_STATIC
#include "TextParser.h"
#include <QFile.h>
#include <QMessageBox.h>
#include <QTextStream.h>
#include <QByteArray.h>

void Menubar::onImportText()
{

    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Open Text"), "", tr("Text Files (*.txt)"));
    for (QString filePath : filePaths) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0, "Error", file.errorString());
            return;
        }

        QTextStream ReadFile(&file);
        QByteArray ba = ReadFile.readAll().toUtf8();
        Parser::TokenizeText(mWorkspace.toStdString(), "testoutput", ba.data());
    }
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
        SettingsDialog* settingsDialog = new SettingsDialog(mSettingsDialogOpened);
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


Menubar::~Menubar()
{
	;
}