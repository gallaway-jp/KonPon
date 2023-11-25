#pragma once
#include <qdialog.h>
#include <QTabWidget.h>
#include <QDialogButtonBox.h>
#include <QCheckBox.h>
class SettingsDialog :
    public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(bool& settingsDialogOpened);

private:
    bool& mSettingsDialogOpened;
    bool hasChanges = false;

    QTabWidget* mTabWidget;
    QDialogButtonBox* mButtonBox;

private:
    void accept();
    void reject();
    void apply();

public:
    void onChange(int slot);
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
    explicit FileTab();
};

class UITab : public QWidget
{
    Q_OBJECT

public:
    explicit UITab(QWidget* parent = nullptr);

signals:
    void onChange(int index);
};

class AnkiTab : public QWidget
{
    Q_OBJECT

public:
    explicit AnkiTab(QWidget* parent = nullptr);
    
signals:
    void onChange(int slot);

private:
    QCheckBox *mEnableAnkiConnectCheckbox;
};