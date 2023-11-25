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
    
private:
    AnkiConnect mAnkiConnect;
    AnkiCardEditor* mAnkiCardEditor = nullptr;
    QPushButton* mSendToAnkiButton = nullptr;
private:
    /*void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;*/
private:
    //QTextEdit* frontTextEdit;
signals:
    
public slots:

private slots:
    void onSaveToLocalButtonClicked();
    void onSendToAnkiButtonClicked();
    void onNoteAdded(bool bAdded);
    void onDeckCreated(bool created);
};