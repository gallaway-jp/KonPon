#pragma once
#include <QDialog>

#include "AnkiConnectLib.h"

#include <string>
#include <unordered_set>

#include <String>

#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE

class AnkiCardEditor;
class Settings;

class CreateAnkiCardDialog :
    public QDialog
{
    Q_OBJECT
public:
    CreateAnkiCardDialog(QWidget* parent, const std::string& kana, const std::string& kanji, Settings* settings);
public:
signals:
    void updateDeckNames(const QStringList& decks);
    void updateNoteTypes(const QStringList& models, const QList<QStringList>& fieldsLists);
    void insertDataIntoField(const QString& data, const QString& field);
    void modelCreated();
public slots:

private:
    std::string mKana;
    std::string mKanji;
    AnkiConnect mAnkiConnect;
    AnkiCardEditor* mAnkiCardEditor = nullptr;
    QPushButton* mSendToAnkiButton = nullptr;
    QStringList mModels;
private slots:
    void onAddKanaToFieldButtonClicked();
    void onAddKanjiToFieldButtonClicked();
    void onSaveToLocalButtonClicked();
    void onSendToAnkiButtonClicked();
    void onGetDeckNames(const QStringList& decks);
    void onGetModelNames(const QStringList& models);
    void onGetModelsFieldNames(const QList<QStringList>& fieldsLists);
    void onNoteAdded(bool bAdded);
    void onModelCreated(bool created);
};