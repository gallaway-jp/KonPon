#pragma once
#include <QDialog>

#include "AnkiConnectLib.h"

#include <string>
#include <unordered_set>

#include <String>

#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QToolBar;
class QWidget;
QT_END_NAMESPACE

class AnkiCardEditor;
class Settings;
class Word;

class CreateAnkiCardDialog :
    public QDialog
{
    Q_OBJECT
public:
    CreateAnkiCardDialog(QWidget* parent, const Word& word, Settings* settings);
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
    std::string m_pitchAccents;
    AnkiConnect mAnkiConnect;
    AnkiCardEditor* mAnkiCardEditor = nullptr;
    QPushButton* mSendToAnkiButton = nullptr;
    QStringList mModels;
private:
    QWidget* createWordInfoWidget(const std::string& data, QPushButton* addToFieldButton);
    void addWordInfoToField(const std::string& data);
private slots:
    void onAddKanaToFieldButtonClicked();
    void onAddKanjiToFieldButtonClicked();
    void onAddPitchAccentsToFieldButtonClicked();
    void onSaveToLocalButtonClicked();
    void onSendToAnkiButtonClicked();
    void onGetDeckNames(const QStringList& decks);
    void onGetModelNames(const QStringList& models);
    void onGetModelsFieldNames(const QList<QStringList>& fieldsLists);
    void onNoteAdded(bool bAdded);
    void onModelCreated(bool created);
};