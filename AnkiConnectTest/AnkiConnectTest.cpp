#pragma warning(disable:4828)
#pragma warning(disable:4819)
#include "AnkiConnectTest.h"


QTEST_MAIN(AnkiConnectTest)
//#include "AnkiConnectTest.moc"

#include <string>

#include <QMap>
#include <QDebug>


#include <QtNetwork/QNetworkProxy>
AnkiConnectTest::AnkiConnectTest()
{
    QNetworkProxy proxy;

    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("localhost");
    proxy.setPort(8888);

    QNetworkProxy::setApplicationProxy(proxy);
}

void AnkiConnectTest::getDeckNamesTest()
{
    QStringList deckNames;
    std::function<void(QStringList)> getDeckNamesSlot = [this, &deckNames](QStringList result) { deckNames = result; mFinished = true; };
    ankiConnect.GetDeckNames(getDeckNamesSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(deckNames.count() > 0, "Failed to retrieve deck names!");
}

void AnkiConnectTest::createDeckTest()
{
    bool deckCreated = false;
    std::function<void(bool)> createDeckSlot = [this, &deckCreated](bool result) { deckCreated = result; mFinished = true; };
    ankiConnect.createDeck("createDeckTestDeck", createDeckSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(deckCreated == true, "Failed to create deck!");
}

void AnkiConnectTest::getModelNamesTest()
{
    QStringList modelNames;
    std::function<void(QStringList)> getModelNamesSlot = [this, &modelNames](QStringList result) { modelNames = result; mFinished = true; };
    ankiConnect.getModelNames(getModelNamesSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(modelNames.count() > 0, "Failed to retrieve model names!");
}

void AnkiConnectTest::getModelFieldNamesTest()
{
    QStringList modelFieldNames;
    std::function<void(QStringList)> getModelFieldNamesSlot = [this, &modelFieldNames](QStringList result) { modelFieldNames = result; mFinished = true; };
    ankiConnect.getModelFieldNames(mModelName, getModelFieldNamesSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(modelFieldNames.count() > 0, "Failed to retrieve model field names!");
}

void AnkiConnectTest::createModelTest()
{
    bool createdModel;
    std::function<void(bool)> createModelSlot = [this, &createdModel](bool result) { createdModel = result; mFinished = true; };
    ankiConnect.createModel("testModel1125_2", 
        { {"field1", CardTemplateFieldType::FrontOnly},
        {"field2", CardTemplateFieldType::BackOnly},
        {"field3", CardTemplateFieldType::BackOnly} },
        createModelSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(createdModel, "Failed to create model!");
}

#include <map>
void AnkiConnectTest::addNotesTest()
{
    bool notesAdded = false;

    QStringList modelFieldNames;
    std::function<void(QStringList)> getModelFieldNamesSlot = [this, &modelFieldNames](QStringList result) { modelFieldNames = result; mFinished = true; };
    ankiConnect.getModelFieldNames(mModelName, getModelFieldNamesSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    std::vector<std::pair<QString, QString>> fieldsMap;
    for(const auto& modelFieldName : modelFieldNames) {
        fieldsMap.push_back({modelFieldName, QString("content of ") + modelFieldName});
    }
    QStringList tags = { "tag1", "tag2" };
    
    std::function<void(bool)> createDeckSlot = [this, &notesAdded](bool result) { notesAdded = result; mFinished = true; };
    ankiConnect.addNote(mDeckName, mModelName, fieldsMap, tags, createDeckSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(notesAdded == true, "Failed to create note!");

    for (const auto& modelFieldName : modelFieldNames) {
        fieldsMap.push_back({ modelFieldName, QString("1 content of ") + modelFieldName });
    }
    std::vector<std::pair<QString, QString>> fieldsMap2;
    for (const auto& modelFieldName : modelFieldNames) {
        fieldsMap2.push_back({ modelFieldName, QString("2 content of ") + modelFieldName });
    }

    ankiConnect.addNotes({
        { mDeckName, mModelName, fieldsMap, tags },
        { mDeckName, mModelName, fieldsMap2, tags }
        }, createDeckSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(notesAdded == true, "Failed to create note!");
}

void AnkiConnectTest::getModelsAndFieldsTest()
{
    QStringList modelNames;
    std::function<void(QStringList)> getModelNamesSlot = [this, &modelNames](QStringList result) { modelNames = result; mFinished = true; };
    ankiConnect.getModelNames(getModelNamesSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(modelNames.count() > 0, "Failed to retrieve model names!");

    QList<QStringList> fieldsLists;
    std::function<void(QList<QStringList>)> getModelsFieldNamesSlot = [this, &fieldsLists](QList<QStringList> result) { fieldsLists = result; mFinished = true; };
    ankiConnect.getModelsFieldNames(modelNames, getModelsFieldNamesSlot);
    while (!mFinished) {
        QCoreApplication::processEvents();
        _sleep(10);
    }
    mFinished = false;

    QVERIFY2(fieldsLists.count() > 0, "Failed to retrieve fields lists!");
}