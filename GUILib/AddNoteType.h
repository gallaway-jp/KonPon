#pragma once
#include <QDialog>

#include <QString>

class QLineEdit;
class QListWidget;
class QTextEdit;

class AddNoteType :
    public QDialog
{
    Q_OBJECT
public:
    AddNoteType(QWidget* parent);
    QString noteType;
    QStringList fields;
    QString frontTemplate;
    QString backTemplate;
private:
    void accept() override;
private:
    QLineEdit* mNoteTypeLineEdit;
    QListWidget* mListWidget;
    QTextEdit* mFrontTemplateEdit;
    QTextEdit* mBackTemplateEdit;
private slots:
    void onCreateFieldButtonClicked();
    void onDeleteFieldButtonClicked();
};