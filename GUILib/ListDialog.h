#pragma once
#include <QDialog>

#include <QString>

class QListWidget;

enum class ListDialogType
{
    ListDialogTypeDefault,
    ListDialogTypeNoteType,
    ListDialogTypeDeck
};

class ListDialog :
    public QDialog
{
    Q_OBJECT
public:
    ListDialog(QWidget* parent, ListDialogType listDialogType, const QStringList& items);
    QStringList result;
private:
    void accept() override;
    void getNewDeckName();
    void getNewNoteType();
private:
    QString mGetTextTitle;
    ListDialogType mListDialogType;
    QListWidget* mListWidget;
private slots:
    void onCreateButtonClicked();
};