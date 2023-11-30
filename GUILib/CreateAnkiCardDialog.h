#pragma once
#include <QDialog>

#include "AnkiConnectLib.h"

#include <string>
#include <unordered_set>

#include <String>

#include <QTextEdit>

class QToolBar;
class AnkiCardEditor;

class CreateAnkiCardDialog :
    public QDialog
{
    Q_OBJECT
public:
    CreateAnkiCardDialog(QWidget* parent, const std::string& kana, const std::string& kanji);
public:
signals:
    void updateDeckNames(const QStringList& decks);
    void updateNoteTypes(const QStringList& models, const QList<QStringList>& fieldsLists);
public slots:

private:
    AnkiConnect mAnkiConnect;
    AnkiCardEditor* mAnkiCardEditor = nullptr;
    QPushButton* mSendToAnkiButton = nullptr;
    QStringList mModels;
private slots:
    void onSaveToLocalButtonClicked();
    void onSendToAnkiButtonClicked();
    void onGetDeckNames(const QStringList& decks);
    void onGetModelNames(const QStringList& models);
    void onGetModelsFieldNames(const QList<QStringList>& fieldsLists);
    void onNoteAdded(bool bAdded);
    void onDeckCreated(bool created);
};