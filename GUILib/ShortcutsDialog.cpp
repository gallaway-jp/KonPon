#include "ShortcutsDialog.h"

#include <QCoreApplication>
#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

ShortcutsDialog::ShortcutsDialog()
	: QDialog(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlag(Qt::WindowMaximizeButtonHint);
	setWindowFlag(Qt::WindowMinimizeButtonHint);

	QVBoxLayout* scrollLayout = new QVBoxLayout(this);
	scrollLayout->setContentsMargins(0, 0, 0, 0);
	scrollLayout->setSpacing(0);
	QVBoxLayout* layout = new QVBoxLayout();

	layout->addLayout(createFileLayout());

	layout->addWidget(createSeparatorWidget());

	layout->addLayout(createToolsLayout());

	layout->addWidget(createSeparatorWidget());

	layout->addLayout(createHelpLayout());

	QWidget* widget = new QWidget();
	widget->setLayout(layout);

	QScrollArea* scrollArea = new QScrollArea();
	scrollLayout->addWidget(scrollArea);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(widget);
	scrollArea->setMinimumWidth((widget->width() * 0.25) + scrollArea->verticalScrollBar()->width());
	scrollArea->setMinimumHeight(widget->width() * 0.25);
}

QFormLayout* ShortcutsDialog::createFileLayout()
{
	QLabel* shortcut = nullptr;
	QFormLayout* fileLayout = new QFormLayout();
	
	m_fileLabel = new QLabel(tr("File"));
	QFont headerFont = m_fileLabel->font();
	headerFont.setPointSize(headerFont.pointSize() + 2);
	headerFont.setBold(true);
	m_fileLabel->setFont(headerFont);
	fileLayout->addRow(m_fileLabel, new QWidget());
	
	m_importTextLabel = new QLabel(tr("Import Text"));
	shortcut = new QLabel("CTRL + O");
	shortcut->setAlignment(Qt::AlignRight);
	fileLayout->addRow(m_importTextLabel, shortcut);

	m_importAudioLabel = new QLabel(tr("Import Audio"));
	m_importAudioLabel->setDisabled(true);
	shortcut = new QLabel("CTRL + A");
	shortcut->setDisabled(true);
	shortcut->setAlignment(Qt::AlignRight);
	fileLayout->addRow(m_importAudioLabel, shortcut);

	m_importTextAudioLabel = new QLabel(tr("Import Text & Audio"));
	m_importTextAudioLabel->setDisabled(true);
	shortcut = new QLabel("CTRL + B");
	shortcut->setDisabled(true);
	shortcut->setAlignment(Qt::AlignRight);
	fileLayout->addRow(m_importTextAudioLabel, shortcut);

	m_viewWordListsLabel = new QLabel(tr("View Word Lists"));
	shortcut = new QLabel("CTRL + W");
	shortcut->setAlignment(Qt::AlignRight);
	fileLayout->addRow(m_viewWordListsLabel, shortcut);
	
	m_exitLabel = new QLabel(tr("Exit"));
	shortcut = new QLabel("CTRL + Q");
	shortcut->setAlignment(Qt::AlignRight);
	fileLayout->addRow(m_exitLabel, shortcut);

	return fileLayout;
}

QFormLayout* ShortcutsDialog::createToolsLayout()
{
	QLabel* shortcut = nullptr;
	QFormLayout* toolsLayout = new QFormLayout();
	
	m_toolsLabel = new QLabel(tr("Tools"));
	QFont headerFont = m_toolsLabel->font();
	headerFont.setPointSize(headerFont.pointSize() + 2);
	headerFont.setBold(true);
	m_toolsLabel->setFont(headerFont);
	toolsLayout->addRow(m_toolsLabel, new QWidget());

	m_syncLabel = new QLabel(tr("Sync"));
	m_syncLabel->setDisabled(true);
	shortcut = new QLabel("CTRL + Y");
	shortcut->setDisabled(true);
	shortcut->setAlignment(Qt::AlignRight);
	toolsLayout->addRow(m_syncLabel, shortcut);
	
	m_settingsLabel = new QLabel(tr("Settings"));
	shortcut = new QLabel("CTRL + T");
	shortcut->setAlignment(Qt::AlignRight);
	toolsLayout->addRow(m_settingsLabel, shortcut);

	return toolsLayout;
}

QFormLayout* ShortcutsDialog::createHelpLayout()
{
	QLabel* shortcut = nullptr;
	QFormLayout* helpLayout = new QFormLayout();

	m_helpLabel = new QLabel(tr("Help"));
	QFont headerFont = m_helpLabel->font();
	headerFont.setPointSize(headerFont.pointSize() + 2);
	headerFont.setBold(true);
	m_helpLabel->setFont(headerFont);
	helpLayout->addRow(m_helpLabel, new QWidget());

	m_manualLabel = new QLabel(tr("Manual"));
	m_manualLabel->setDisabled(true);
	shortcut = new QLabel("CTRL + M");
	shortcut->setDisabled(true);
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_manualLabel, shortcut);
	
	m_releaseNotesLabel = new QLabel(tr("Release Notes"));
	m_releaseNotesLabel->setDisabled(true);
	shortcut = new QLabel("CTRL + N");
	shortcut->setDisabled(true);
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_releaseNotesLabel, shortcut);

	m_videoTutorialsLabel = new QLabel(tr("Video Tutorials"));
	shortcut = new QLabel("CTRL + V");
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_videoTutorialsLabel, shortcut);

	m_keyboardShortcutsLabel = new QLabel(tr("Keyboard Shortcuts"));
	shortcut = new QLabel("CTRL + K");
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_keyboardShortcutsLabel, shortcut);

	m_viewGithubLabel = new QLabel(tr("View Github"));
	shortcut = new QLabel("CTRL + H");
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_viewGithubLabel, shortcut);

	m_checkUpdatesLabel = new QLabel(tr("Check for Updates"));
	m_checkUpdatesLabel->setDisabled(true);
	shortcut = new QLabel("CTRL + U");
	shortcut->setDisabled(true);
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_checkUpdatesLabel, shortcut);

	m_aboutLabel = new QLabel(tr("About"));
	shortcut = new QLabel("CTRL + I");
	shortcut->setAlignment(Qt::AlignRight);
	helpLayout->addRow(m_aboutLabel, shortcut);

	return helpLayout;
}

QWidget* ShortcutsDialog::createSeparatorWidget()
{
	QFrame* separator = new QFrame();
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);
	return separator;
}

void ShortcutsDialog::reject()
{
	QDialog::reject();
}

void ShortcutsDialog::onRetranslateUI()
{
	m_fileLabel->setText(QCoreApplication::translate("ShortcutsDialog", "File"));
	m_importTextLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Import Text"));
	m_importAudioLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Import Audio"));
	m_importTextAudioLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Import Text & Audio"));
	m_viewWordListsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "View Word Lists"));
	m_exitLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Exit"));

	m_toolsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Tools"));
	m_syncLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Sync"));
	m_settingsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Settings"));

	m_helpLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Help"));
	m_manualLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Manual"));
	m_releaseNotesLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Release Notes"));
	m_videoTutorialsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Video Tutorials"));
	m_keyboardShortcutsLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Keyboard Shortcuts"));
	m_viewGithubLabel->setText(QCoreApplication::translate("ShortcutsDialog", "View Github"));
	m_checkUpdatesLabel->setText(QCoreApplication::translate("ShortcutsDialog", "Check for Updates"));
	m_aboutLabel->setText(QCoreApplication::translate("ShortcutsDialog", "About"));
}

ShortcutsDialog::~ShortcutsDialog()
{
	emit closeDialog();
}