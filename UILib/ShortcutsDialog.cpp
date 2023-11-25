#include "ShortcutsDialog.h"

#include "Strings.h"

#include "QFormLayout.h"
#include "QLabel.h"

ShortcutsDialog::ShortcutsDialog(bool &keyboardShortcutsDialogOpened)
	: mKeyboardShortcutsDialogOpened(keyboardShortcutsDialogOpened), QDialog(nullptr)
{
	//QWidget* widget = new QWidget(this);
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	layout->addRow(tr(DIALOG_SHORTCUTS_IMPORT_TEXT), new QLabel(DIALOG_SHORTCUTS_IMPORT_TEXT_SHORTCUT));
	layout->addRow(tr(DIALOG_SHORTCUTS_IMPORT_AUDIO), new QLabel(DIALOG_SHORTCUTS_IMPORT_AUDIO_SHORTCUT));
	layout->addRow(tr(DIALOG_SHORTCUTS_KEYBOARD_SHORTCUTS), new QLabel(DIALOG_SHORTCUTS_KEYBOARD_SHORTCUTS_SHORTCUT));

	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void ShortcutsDialog::reject()
{
	mKeyboardShortcutsDialogOpened = false;
	QDialog::reject();
}