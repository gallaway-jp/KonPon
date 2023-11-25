#include "SettingsDialog.h"

#include "Settings.h"

#include "QTabWidget.h"
#include "QDialogButtonBox.h"
#include "QCheckBox.h"
#include "QPushButton.h"
#include "QFormLayout.h"
#include "QLabel.h"
#include "QComboBox.h"
#include "QLineEdit.h"

SettingsDialog::SettingsDialog(Settings* settings, bool& settingsDialogOpened)
	: mSettingsDialogOpened(settingsDialogOpened),
	QDialog(nullptr)
{
	mTabWidget = new QTabWidget;
	mTabWidget->addTab(new GeneralTab(), tr("General"));
	mTabWidget->addTab(new FileTab(settings), tr("File"));
	UITab* uiTab = new UITab;
	mTabWidget->addTab(uiTab, tr("UI"));
	connect(uiTab, &UITab::onChange, this, &SettingsDialog::onChange);
	AnkiTab* ankiTab = new AnkiTab;
	mTabWidget->addTab(ankiTab, tr("Anki"));
	connect(ankiTab, &AnkiTab::onChange, this, &SettingsDialog::onChange);

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
	connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(mButtonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mTabWidget);
	mainLayout->addWidget(mButtonBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Settings"));
	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void SettingsDialog::accept()
{
	mSettingsDialogOpened = false;
	//Do Settings
	QDialog::accept();
}

void SettingsDialog::reject()
{
	mSettingsDialogOpened = false;
	QDialog::reject();
}

void SettingsDialog::apply()
{
	hasChanges = false;
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);
	//Do Settings
}

void SettingsDialog::onChange(int slot)
{
	hasChanges = true;
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);
}

GeneralTab::GeneralTab(QWidget* parent)
	: QWidget(parent)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	layout->addRow(tr("Import Text"), new QLabel("CTRL + O"));
	layout->addRow(tr("Import Audio"), new QLabel("CTRL + A"));
	layout->addRow(tr("Keyboard Shortcuts"), new QLabel("CTRL + K"));
}

FileTab::FileTab(Settings* settings)
	: QWidget(nullptr)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	layout->addRow(tr("Workspace"), new QLineEdit(settings->mFile.workspace));
}

UITab::UITab(QWidget* parent)
	: QWidget(parent)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	QComboBox* themeCombo = new QComboBox;
	themeCombo->setEditable(false);
	themeCombo->addItem(tr("Default"));
	themeCombo->addItem(tr("Light"));
	themeCombo->addItem(tr("Dark"));
	connect(themeCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	QComboBox* languageCombo = new QComboBox;
	languageCombo->setEditable(false);
	languageCombo->addItem(tr("Default"));
	languageCombo->addItem(tr("English"));
	languageCombo->addItem(tr("Japanese"));
	connect(languageCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	QComboBox* textSizeCombo = new QComboBox;
	textSizeCombo->setEditable(false);
	textSizeCombo->addItem(tr("Small"));
	textSizeCombo->addItem(tr("Medium"));
	textSizeCombo->addItem(tr("Large"));
	connect(textSizeCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	layout->addRow(tr("Theme"), themeCombo);
	layout->addRow(tr("Language"), languageCombo);
	layout->addRow(tr("Text Size"), textSizeCombo);

	QT_TR_NOOP("");
}

AnkiTab::AnkiTab(QWidget* parent)
	: QWidget(parent)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	mEnableAnkiConnectCheckbox = new QCheckBox(tr("Enable Anki Connect Feature"));
	connect(mEnableAnkiConnectCheckbox, &QCheckBox::stateChanged, this, &AnkiTab::onChange);

	layout->addRow(mEnableAnkiConnectCheckbox);
}