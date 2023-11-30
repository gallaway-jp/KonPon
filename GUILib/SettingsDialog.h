#pragma once
#include <qdialog.h>

class QString;
class QTabWidget;
class QDialogButtonBox;
class QCheckBox;
class QLineEdit;

class Settings;

class SettingsDialog :
    public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(Settings* settings, bool& settingsDialogOpened);

private:
    bool& mSettingsDialogOpened;
    bool hasChanges = false;

    QTabWidget* mTabWidget;
    QDialogButtonBox* mButtonBox;

private:
    void accept();
    void reject();
    void apply();

signals:
    void applySettings();

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
    explicit UITab(QWidget* parent = nullptr);

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