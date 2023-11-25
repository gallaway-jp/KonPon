#pragma once
#include <QtTest/QtTest>
#include "AnkiConnectLib.h"
#include <QString>
class AnkiConnectTest : public QObject
{
    Q_OBJECT
public:
    AnkiConnectTest();
private:
    AnkiConnect ankiConnect;
    QString mDeckName = "createDeckTestDeck";
    QString mModelName = "_TestType_";
    bool mFinished = false;
private slots:
    void getDeckNamesTest();
    void createDeckTest();
    void getModelNamesTest();
    void getModelFieldNamesTest();
    void createModelTest();
    void addNotesTest();
    void getModelsAndFieldsTest();
};