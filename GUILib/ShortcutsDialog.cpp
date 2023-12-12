#include "ShortcutsDialog.h"

#include <QCoreApplication>
#include <QFormLayout>
#include <QLabel>

ShortcutsDialog::ShortcutsDialog()
	: QDialog(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	m_importTextLabel = new QLabel(tr("Import Text"));
	layout->addRow(m_importTextLabel, new QLabel("CTRL + O"));
	m_importAudioLabel = new QLabel(tr("Import Audio"));
	layout->addRow(m_importAudioLabel, new QLabel("CTRL + A"));
	m_settingsLabel = new QLabel(tr("Settings"));
	layout->addRow(m_settingsLabel, new QLabel("CTRL + T"));
	m_keyboardShortcutsLabel = new QLabel(tr("Keyboard Shortcuts"));
	layout->addRow(m_keyboardShortcutsLabel, new QLabel("CTRL + K"));

	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void ShortcutsDialog::reject()
{
	QDialog::reject();
}

void ShortcutsDialog::onRetranslateUI()
{
	m_importTextLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Import Text"));
	m_importAudioLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Import Audio"));
	m_settingsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Settings"));
	m_keyboardShortcutsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Keyboard Shortcuts"));
}

ShortcutsDialog::~ShortcutsDialog()
{
	emit closeDialog();
}