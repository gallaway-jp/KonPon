#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
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
    QLabel* m_importTextLabel = nullptr;
    QLabel* m_importAudioLabel = nullptr;
    QLabel* m_settingsLabel = nullptr;
    QLabel* m_keyboardShortcutsLabel = nullptr;

private:
    void reject();
};

