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
	FileTab* fileTab = new FileTab(settings);
	mTabWidget->addTab(fileTab, tr("File"));
	connect(fileTab, &FileTab::onChange, this, &SettingsDialog::onChange);

	UITab* uiTab = new UITab;
	mTabWidget->addTab(uiTab, tr("UI"));
	connect(uiTab, &UITab::onChange, this, &SettingsDialog::onChange);
	AnkiTab* ankiTab = new AnkiTab(settings);
	mTabWidget->addTab(ankiTab, tr("Anki"));
	connect(ankiTab, &AnkiTab::onChange, this, &SettingsDialog::onChange);

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
	connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(mButtonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);

	connect(this, &SettingsDialog::applySettings, fileTab, &FileTab::onApplySettings);
	connect(this, &SettingsDialog::applySettings, ankiTab, &AnkiTab::onApplySettings);

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

UITab::UITab(QWidget* parent)
	: QWidget(parent)
{
	/*QFormLayout* layout = new QFormLayout(this);
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
	layout->addRow(tr("Text Size"), textSizeCombo);*/
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