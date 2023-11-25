#include "ShortcutsDialog.h"

#include "QFormLayout.h"
#include "QLabel.h"

ShortcutsDialog::ShortcutsDialog(bool& keyboardShortcutsDialogOpened)
	: mKeyboardShortcutsDialogOpened(keyboardShortcutsDialogOpened), QDialog(nullptr)
{
	//QWidget* widget = new QWidget(this);
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	layout->addRow(tr("Import Text"), new QLabel("CTRL + O"));
	layout->addRow(tr("Import Audio"), new QLabel("CTRL + A"));
	layout->addRow(tr("Keyboard Shortcuts"), new QLabel("CTRL + K"));

	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void ShortcutsDialog::reject()
{
	mKeyboardShortcutsDialogOpened = false;
	QDialog::reject();
}