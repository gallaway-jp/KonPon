#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
class QFormLayout;
class QLabel;
QT_END_NAMESPACE

class ShortcutsDialog :
    public QDialog
{
    Q_OBJECT
public:
    ShortcutsDialog();
    ~ShortcutsDialog();

signals:
    void closeDialog();

public slots:
    void onRetranslateUI();

private:
    QLabel* m_fileLabel = nullptr;
    QLabel* m_importTextLabel = nullptr;
    QLabel* m_importAudioLabel = nullptr;
    QLabel* m_importTextAudioLabel = nullptr;
    QLabel* m_viewWordListsLabel = nullptr;
    QLabel* m_exitLabel = nullptr;

    QLabel* m_toolsLabel = nullptr;
    QLabel* m_syncLabel = nullptr;
    QLabel* m_settingsLabel = nullptr;

    QLabel* m_helpLabel = nullptr;
    QLabel* m_manualLabel = nullptr;
    QLabel* m_releaseNotesLabel = nullptr;
    QLabel* m_videoTutorialsLabel = nullptr;
    QLabel* m_keyboardShortcutsLabel = nullptr;
    QLabel* m_viewGithubLabel = nullptr;
    QLabel* m_checkUpdatesLabel = nullptr;
    QLabel* m_aboutLabel = nullptr;

private:
    QFormLayout* createFileLayout();
    QFormLayout* createToolsLayout();
    QFormLayout* createHelpLayout();
    QWidget* createSeparatorWidget();
    void reject();
};

