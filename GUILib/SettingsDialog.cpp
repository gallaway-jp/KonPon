#include "SettingsDialog.h"

#include "Settings.h"

#include <QTabWidget>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>

SettingsDialog::SettingsDialog(Settings* settings, bool& settingsDialogOpened)
	: mSettingsDialogOpened(settingsDialogOpened), m_settings(settings),
	QDialog(nullptr)
{
	mTabWidget = new QTabWidget;
	mTabWidget->addTab(new GeneralTab(), tr("General"));
	FileTab* fileTab = new FileTab(settings);
	mTabWidget->addTab(fileTab, tr("File"));
	connect(fileTab, &FileTab::onChange, this, &SettingsDialog::onChange);

	UITab* uiTab = new UITab(settings);
	mTabWidget->addTab(uiTab, tr("UI"));
	connect(uiTab, &UITab::onChange, this, &SettingsDialog::onChange);
	AnkiTab* ankiTab = new AnkiTab(settings);
	mTabWidget->addTab(ankiTab, tr("Anki"));
	connect(ankiTab, &AnkiTab::onChange, this, &SettingsDialog::onChange);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults);
	QPushButton* clearDataButton = buttonBox->addButton(tr("Clear Data"), QDialogButtonBox::ActionRole);
	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &SettingsDialog::restoreAllDefaults);
	connect(clearDataButton, &QPushButton::clicked, this, &SettingsDialog::clearData);

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
	connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(mButtonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);

	connect(this, &SettingsDialog::applySettings, fileTab, &FileTab::onApplySettings);
	connect(this, &SettingsDialog::applySettings, uiTab, &UITab::onApplySettings);
	connect(this, &SettingsDialog::applySettings, ankiTab, &AnkiTab::onApplySettings);

	connect(this, &SettingsDialog::restoreDefaults, fileTab, &FileTab::onRestoreDefaults);
	connect(this, &SettingsDialog::restoreDefaults, uiTab, &UITab::onRestoreDefaults);
	connect(this, &SettingsDialog::restoreDefaults, ankiTab, &AnkiTab::onRestoreDefaults);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mTabWidget);
	mainLayout->addWidget(buttonBox);
	mainLayout->addWidget(mButtonBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Settings"));
	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void SettingsDialog::accept()
{
	mSettingsDialogOpened = false;
	emit applySettings();
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
	emit applySettings();
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);
}

void SettingsDialog::clearData()
{
	int result = QMessageBox::warning(this, tr("Clearing Data"),
		tr("You are about to clear all data including\nimported texts, word lists, and settings.\nAre you sure?"),
		QMessageBox::Yes | QMessageBox::No);
	if (result == QMessageBox::Yes) {
		QMessageBox::information(this, tr("Clearing Data"), tr("Data will be cleared when KonPon is closed."));
		m_settings->clearDataOnTermination();
	}
}

void SettingsDialog::restoreAllDefaults()
{
	emit restoreDefaults();
	onChange();
}

void SettingsDialog::onChange()
{
	hasChanges = true;
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);
}

GeneralTab::GeneralTab(QWidget* parent)
	: QWidget(parent)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);
}

FileTab::FileTab(Settings* settings)
	: QWidget(nullptr), mSettings(settings)
{

	QVBoxLayout* layout = new QVBoxLayout(this);
	
	QHBoxLayout* workspaceLayout = new QHBoxLayout();
	mWorkspaceLineEdit = new QLineEdit(settings->mFile.workspace);
	QPushButton* changeWorkspaceButton = new QPushButton(tr("Specify Workspace"));
	connect(mWorkspaceLineEdit, &QLineEdit::textChanged, this, &FileTab::onChange);
	connect(changeWorkspaceButton, &QPushButton::clicked, this, &FileTab::onChangeWorkspaceButtonClicked);
	
	workspaceLayout->addWidget(mWorkspaceLineEdit);
	workspaceLayout->addWidget(changeWorkspaceButton);

	layout->addLayout(workspaceLayout);	
}

#include <QFileDialog>
void FileTab::onChangeWorkspaceButtonClicked()
{
	QFileDialog dialog;
	QString directory = dialog.getExistingDirectory();
	if (!directory.isEmpty()) {
		mWorkspaceLineEdit->setText(directory);
	}
}

void FileTab::onApplySettings()
{
	mSettings->mFile.setWorkspace(mWorkspaceLineEdit->text());
}

void FileTab::onRestoreDefaults()
{
	mWorkspaceLineEdit->setText(mSettings->mFile.defaultWorkspace);
}

UITab::UITab(Settings* settings)
	: QWidget(nullptr), mSettings(settings)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	m_themeCombo = new QComboBox;
	m_themeCombo->setEditable(false);
	m_themeCombo->addItem(tr("Default"), static_cast<int>(Settings::Theme::Default));
	m_themeCombo->addItem(tr("Light"), static_cast<int>(Settings::Theme::Light));
	m_themeCombo->addItem(tr("Dark"), static_cast<int>(Settings::Theme::Dark));
	m_themeCombo->setCurrentIndex(m_themeCombo->findData(static_cast<int>(mSettings->ui.getTheme())));
	connect(m_themeCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	/*QComboBox* languageCombo = new QComboBox;
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
	connect(textSizeCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);*/

	layout->addRow(tr("Theme"), m_themeCombo);
	/*layout->addRow(tr("Language"), languageCombo);
	layout->addRow(tr("Text Size"), textSizeCombo);*/
}

void UITab::onApplySettings()
{
	mSettings->ui.setTheme(static_cast<Settings::Theme>(m_themeCombo->currentData().toInt()));
}

void UITab::onRestoreDefaults()
{
	m_themeCombo->setCurrentIndex(m_themeCombo->findData(static_cast<int>(Settings::Theme::Default)));
}

AnkiTab::AnkiTab(Settings* settings)
	: QWidget(nullptr), mSettings(settings)
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

	mEnableAnkiConnectCheckbox = new QCheckBox(tr("Enable Anki Connect Feature"));
	mEnableAnkiConnectCheckbox->setChecked(mSettings->mAnki.isAnkiConnectFeatureEnabled);
	connect(mEnableAnkiConnectCheckbox, &QCheckBox::stateChanged, this, &AnkiTab::onChange);

	layout->addRow(mEnableAnkiConnectCheckbox);
}

void AnkiTab::onApplySettings()
{
	mSettings->mAnki.setEnableAnkiConnectFeature(mEnableAnkiConnectCheckbox->isChecked());
}

void AnkiTab::onRestoreDefaults()
{
	mEnableAnkiConnectCheckbox->setChecked(true);
}