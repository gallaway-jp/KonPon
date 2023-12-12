#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QString;
class QTabWidget;
QT_END_NAMESPACE

class AnkiTab;
class FileTab;
class GeneralTab;
class Settings;
class UITab;

class SettingsDialog :
    public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(Settings* settings);
    ~SettingsDialog();

private:
    Settings* m_settings = nullptr;
    bool hasChanges = false;

    QTabWidget* mTabWidget;
    QDialogButtonBox* mButtonBox;
    GeneralTab* m_generalTab = nullptr;
    FileTab* m_fileTab = nullptr;
    UITab* m_uiTab = nullptr;
    AnkiTab* m_ankiTab = nullptr;
    QPushButton* m_clearDataButton = nullptr;

private:
    void accept();
    void reject();
    void apply();
    void clearData();
    void restoreAllDefaults();

signals:
    void applySettings();
    void restoreDefaults();
    void retranslateUI();
    void closeDialog();

public slots:
    void onChange();
    void onRetranslateUI();
};

class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralTab(QWidget* parent = nullptr);
};

class FileTab : public QWidget
{
    Q_OBJECT

public:
    explicit FileTab(Settings* settings);
signals:
    void onChange();
public slots:
    void onApplySettings();
    void onRestoreDefaults();
    void onRetranslateUI();
private:
    Settings* mSettings = nullptr;
    QLineEdit* mWorkspaceLineEdit = nullptr;
    QPushButton* m_changeWorkspaceButton = nullptr;
private slots:
    void onChangeWorkspaceButtonClicked();
};

class UITab : public QWidget
{
    Q_OBJECT

public:
    explicit UITab(Settings* settings);
public slots:
    void onApplySettings();
    void onRestoreDefaults();
    void onRetranslateUI();
private:
    Settings* mSettings = nullptr;
    QComboBox* m_themeCombo = nullptr;
    QComboBox* m_languageCombo = nullptr;
    QLabel* m_themeLabel = nullptr;
    QLabel* m_languageLabel = nullptr;
signals:
    void onChange();
    void retranslateUI();
};

class AnkiTab : public QWidget
{
    Q_OBJECT

public:
    explicit AnkiTab(Settings* settings);

signals:
    void onChange();
public slots:
    void onApplySettings();
    void onRestoreDefaults();
    void onRetranslateUI();
private:
    Settings* mSettings = nullptr;
    QCheckBox* mEnableAnkiConnectCheckbox;
};