#pragma once
#include <qdialog.h>

QT_BEGIN_NAMESPACE
class QString;
class QTabWidget;
class QDialogButtonBox;
class QCheckBox;
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

class Settings;

class SettingsDialog :
    public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(Settings* settings, bool& settingsDialogOpened);

private:
    Settings* m_settings = nullptr;
    bool& mSettingsDialogOpened;
    bool hasChanges = false;

    QTabWidget* mTabWidget;
    QDialogButtonBox* mButtonBox;

private:
    void accept();
    void reject();
    void apply();
    void clearData();

signals:
    void applySettings();
    void restoreDefaults();

public slots:
    void onChange();
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
private:
    Settings* mSettings = nullptr;
    QLineEdit* mWorkspaceLineEdit = nullptr;
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
private:
    Settings* mSettings = nullptr;
    QComboBox* m_themeCombo = nullptr;
signals:
    void onChange();
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

private:
    Settings* mSettings = nullptr;
    QCheckBox* mEnableAnkiConnectCheckbox;
};