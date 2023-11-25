#include "SettingsDialog.h"

#include "Strings.h"

#include "QTabWidget.h"
#include "QDialogButtonBox.h"
#include "QPushButton.h"
#include "QFormLayout.h"
#include "QLabel.h"
#include "QComboBox.h"

SettingsDialog::SettingsDialog(bool& settingsDialogOpened)
	: mSettingsDialogOpened(settingsDialogOpened),
	QDialog(nullptr)
{
	mTabWidget = new QTabWidget;
	mTabWidget->addTab(new GeneralTab(), tr(DIALOG_SETTINGS_TAB_GENERAL));
	mTabWidget->addTab(new FileTab(), tr(DIALOG_SETTINGS_TAB_FILE));
	UITab* uiTab = new UITab;
	mTabWidget->addTab(uiTab, tr(DIALOG_SETTINGS_TAB_UI));
	connect(uiTab, &UITab::onChange, this, &SettingsDialog::onChange);
	AnkiTab *ankiTab = new AnkiTab;
	mTabWidget->addTab(ankiTab, tr(DIALOG_SETTINGS_TAB_ANKI));
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

	setWindowTitle(tr(DIALOG_SETTINGS));
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

	layout->addRow(tr(DIALOG_SHORTCUTS_IMPORT_TEXT), new QLabel(DIALOG_SHORTCUTS_IMPORT_TEXT_SHORTCUT));
	layout->addRow(tr(DIALOG_SHORTCUTS_IMPORT_AUDIO), new QLabel(DIALOG_SHORTCUTS_IMPORT_AUDIO_SHORTCUT));
	layout->addRow(tr(DIALOG_SHORTCUTS_KEYBOARD_SHORTCUTS), new QLabel(DIALOG_SHORTCUTS_KEYBOARD_SHORTCUTS_SHORTCUT));
}

FileTab::FileTab()
	: QWidget(nullptr)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	layout->addRow(tr(DIALOG_SHORTCUTS_IMPORT_TEXT), new QLabel(DIALOG_SHORTCUTS_IMPORT_TEXT_SHORTCUT));
	layout->addRow(tr(DIALOG_SHORTCUTS_IMPORT_AUDIO), new QLabel(DIALOG_SHORTCUTS_IMPORT_AUDIO_SHORTCUT));
	layout->addRow(tr(DIALOG_SHORTCUTS_KEYBOARD_SHORTCUTS), new QLabel(DIALOG_SHORTCUTS_KEYBOARD_SHORTCUTS_SHORTCUT));
}

UITab::UITab(QWidget* parent)
	: QWidget(parent)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	QComboBox *themeCombo = new QComboBox;
	themeCombo->setEditable(false);
	themeCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_THEME_DEFAULT));
	themeCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_THEME_LIGHT));
	themeCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_THEME_DARK));
	connect(themeCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);
	
	QComboBox* languageCombo = new QComboBox;
	languageCombo->setEditable(false);
	languageCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_LANGUAGE_DEFAULT));
	languageCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_LANGUAGE_ENGLISH));
	languageCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_LANGUAGE_JAPANESE));
	connect(languageCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	QComboBox* textSizeCombo = new QComboBox;
	textSizeCombo->setEditable(false);
	textSizeCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_TEXT_SIZE_SMALL));
	textSizeCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_TEXT_SIZE_MEDIUM));
	textSizeCombo->addItem(tr(DIALOG_SETTINGS_TAB_UI_TEXT_SIZE_LARGE));
	connect(textSizeCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	layout->addRow(tr(DIALOG_SETTINGS_TAB_UI_THEME), themeCombo);
	layout->addRow(tr(DIALOG_SETTINGS_TAB_UI_LANGUAGE), languageCombo);
	layout->addRow(tr(DIALOG_SETTINGS_TAB_UI_TEXT_SIZE), textSizeCombo);

	QT_TR_NOOP("");
}

AnkiTab::AnkiTab(QWidget* parent)
	: QWidget(parent)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	mEnableAnkiConnectCheckbox = new QCheckBox(tr(DIALOG_SETTINGS_TAB_ANKI_CONNECT));
	connect(mEnableAnkiConnectCheckbox, &QCheckBox::stateChanged, this, &AnkiTab::onChange);

	layout->addRow(mEnableAnkiConnectCheckbox);
}

