#pragma once
#include <QDialog>

#include <QString>

class QLineEdit;
class QListWidget;

class AddNoteType :
    public QDialog
{
    Q_OBJECT
public:
    AddNoteType(QWidget* parent);
    QString noteType;
    QStringList fields;
private:
    void accept() override;
private:
    QLineEdit* mNoteTypeLineEdit;
    QListWidget* mListWidget;
private slots:
    void onCreateFieldButtonClicked();
    void onDeleteFieldButtonClicked();
};