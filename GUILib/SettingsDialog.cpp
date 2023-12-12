#include "SettingsDialog.h"

#include "Settings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>

SettingsDialog::SettingsDialog(Settings* settings)
	: m_settings(settings),
	QDialog(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);

	mTabWidget = new QTabWidget;
	m_generalTab = new GeneralTab();
	mTabWidget->addTab(m_generalTab, tr("General"));
	m_fileTab = new FileTab(settings);
	mTabWidget->addTab(m_fileTab, tr("File"));
	connect(m_fileTab, &FileTab::onChange, this, &SettingsDialog::onChange);

	m_uiTab = new UITab(settings);
	mTabWidget->addTab(m_uiTab, tr("UI"));
	connect(m_uiTab, &UITab::onChange, this, &SettingsDialog::onChange);
	connect(m_uiTab, &UITab::retranslateUI, this, &SettingsDialog::retranslateUI);

	m_ankiTab = new AnkiTab(settings);
	mTabWidget->addTab(m_ankiTab, tr("Anki"));
	connect(m_ankiTab, &AnkiTab::onChange, this, &SettingsDialog::onChange);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults);
	m_clearDataButton = buttonBox->addButton(tr("Clear Data"), QDialogButtonBox::ActionRole);
	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &SettingsDialog::restoreAllDefaults);
	connect(m_clearDataButton, &QPushButton::clicked, this, &SettingsDialog::clearData);

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
	connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(mButtonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	mButtonBox->button(QDialogButtonBox::Apply)->setDisabled(!hasChanges);

	connect(this, &SettingsDialog::applySettings, m_fileTab, &FileTab::onApplySettings);
	connect(this, &SettingsDialog::applySettings, m_uiTab, &UITab::onApplySettings);
	connect(this, &SettingsDialog::applySettings, m_ankiTab, &AnkiTab::onApplySettings);

	connect(this, &SettingsDialog::restoreDefaults, m_fileTab, &FileTab::onRestoreDefaults);
	connect(this, &SettingsDialog::restoreDefaults, m_uiTab, &UITab::onRestoreDefaults);
	connect(this, &SettingsDialog::restoreDefaults, m_ankiTab, &AnkiTab::onRestoreDefaults);

	connect(this, &SettingsDialog::retranslateUI, this, &SettingsDialog::onRetranslateUI);
	connect(this, &SettingsDialog::retranslateUI, m_fileTab, &FileTab::onRetranslateUI);
	connect(this, &SettingsDialog::retranslateUI, m_uiTab, &UITab::onRetranslateUI);
	connect(this, &SettingsDialog::retranslateUI, m_ankiTab, &AnkiTab::onRetranslateUI);

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
	emit applySettings();
	QDialog::accept();
}

void SettingsDialog::reject()
{
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

void SettingsDialog::onRetranslateUI()
{
	setWindowTitle(QCoreApplication::translate("SettingsDialog", "Settings"));
	mTabWidget->setTabText(mTabWidget->indexOf(m_generalTab), QCoreApplication::translate("SettingsDialog", "General"));
	mTabWidget->setTabText(mTabWidget->indexOf(m_fileTab), QCoreApplication::translate("SettingsDialog", "File"));
	mTabWidget->setTabText(mTabWidget->indexOf(m_uiTab), QCoreApplication::translate("SettingsDialog", "UI"));
	mTabWidget->setTabText(mTabWidget->indexOf(m_ankiTab), QCoreApplication::translate("SettingsDialog", "Anki"));
	m_clearDataButton->setText(QCoreApplication::translate("SettingsDialog", "Clear Data"));
}

SettingsDialog::~SettingsDialog()
{
	emit closeDialog();
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
	m_changeWorkspaceButton = new QPushButton(tr("Specify Workspace"));
	connect(mWorkspaceLineEdit, &QLineEdit::textChanged, this, &FileTab::onChange);
	connect(m_changeWorkspaceButton, &QPushButton::clicked, this, &FileTab::onChangeWorkspaceButtonClicked);
	
	workspaceLayout->addWidget(mWorkspaceLineEdit);
	workspaceLayout->addWidget(m_changeWorkspaceButton);

	layout->addLayout(workspaceLayout);	
}

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

void FileTab::onRetranslateUI()
{
	m_changeWorkspaceButton->setText(QCoreApplication::translate("FileTab", "Specify Workspace"));
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

	m_languageCombo = new QComboBox;
	m_languageCombo->setEditable(false);
	m_languageCombo->addItem(tr("Default"), static_cast<int>(Settings::Language::Default));
	m_languageCombo->addItem(tr("English"), static_cast<int>(Settings::Language::English));
	m_languageCombo->addItem(tr("Japanese"), static_cast<int>(Settings::Language::Japanese));
	m_languageCombo->setCurrentIndex(m_themeCombo->findData(static_cast<int>(mSettings->ui.getLanguage())));
	connect(m_languageCombo, &QComboBox::currentIndexChanged, this, &UITab::onChange);

	m_themeLabel = new QLabel(tr("Theme"));
	layout->addRow(m_themeLabel, m_themeCombo);
	m_languageLabel = new QLabel(tr("Language"));
	layout->addRow(m_languageLabel, m_languageCombo);
}

void UITab::onApplySettings()
{
	mSettings->ui.setTheme(static_cast<Settings::Theme>(m_themeCombo->currentData().toInt()));
	mSettings->ui.setLanguage(static_cast<Settings::Language>(m_languageCombo->currentData().toInt()));
	emit retranslateUI();
}

void UITab::onRestoreDefaults()
{
	m_themeCombo->setCurrentIndex(m_themeCombo->findData(static_cast<int>(Settings::Theme::Default)));
	m_languageCombo->setCurrentIndex(m_languageCombo->findData(static_cast<int>(Settings::Language::Default)));
}

void UITab::onRetranslateUI()
{
	m_themeLabel->setText(QCoreApplication::translate("UITab", "Theme"));
	m_themeCombo->setItemText(m_themeCombo->findData(static_cast<int>(Settings::Theme::Default)),
		QCoreApplication::translate("UITab", "Default"));
	m_themeCombo->setItemText(m_themeCombo->findData(static_cast<int>(Settings::Theme::Light)),
		QCoreApplication::translate("UITab", "Light"));
	m_themeCombo->setItemText(m_themeCombo->findData(static_cast<int>(Settings::Theme::Dark)),
		QCoreApplication::translate("UITab", "Dark"));

	m_languageLabel->setText(QCoreApplication::translate("UITab", "Language"));
	m_languageCombo->setItemText(m_languageCombo->findData(static_cast<int>(Settings::Language::Default)),
		QCoreApplication::translate("UITab", "Default"));
	m_languageCombo->setItemText(m_languageCombo->findData(static_cast<int>(Settings::Language::English)),
		QCoreApplication::translate("UITab", "English"));
	m_languageCombo->setItemText(m_languageCombo->findData(static_cast<int>(Settings::Language::Japanese)),
		QCoreApplication::translate("UITab", "Japanese"));
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

void AnkiTab::onRetranslateUI()
{
	mEnableAnkiConnectCheckbox->setText(QCoreApplication::translate("AnkiTab", "Enable Anki Connect Feature"));
}