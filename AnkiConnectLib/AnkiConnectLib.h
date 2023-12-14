#pragma once

#include "ankiconnectlib_global.h"

#include "RequestManager.h"

#include <vector>

#include <QMap>
#include <QObject>
#include <QString>

struct ANKICONNECTLIB_EXPORT NoteData
{
	QString deckName;
	QString modelName;
	std::vector<std::pair<QString, QString>> fields;
	QStringList tags;
};

struct NewCardTypeData
{
	QStringList fields;
	QString frontTemplate;
	QString backTemplate;
};

/*!
	\class AnkiConnect

	\brief The AnkiConnect class provides methods
	to interact with the Anki API exposed by the AnkiConnect plugin.
*/
class ANKICONNECTLIB_EXPORT AnkiConnect : public QObject
{
	Q_OBJECT
public:
	AnkiConnect(const QString& address = "http://localhost", const QString& port = "8765");

	void GetDeckNames(std::function<void(QStringList)>& slot);

	void createDeck(QString deckName, std::function<void(bool)>& slot);

	void getModelNames(std::function<void(QStringList)>& slot);

	void getModelFieldNames(QString modelName, std::function<void(QStringList)>& slot);

	void getModelsFieldNames(QStringList modelNames, std::function<void(QList<QStringList>)>& slot);

	void createModel(QString modelName, NewCardTypeData newCardTypeData, std::function<void(bool)>& slot);
	
	void addNote(QString deckName, QString modelName, std::vector<std::pair<QString, QString>> fields, QStringList tags, std::function<void(bool)>& slot);

	void addNotes(const std::vector<NoteData>& notesData, std::function<void(bool)>& slot);
	
signals:
	void getDeckNamesResponse(QStringList deckNames);
	void createDeckResponse(bool deckCreated);
	void getModelNamesResponse(QStringList modelNames);
	void getModelFieldNamesResponse(QStringList modelFieldNames);
	void getModelsFieldNamesResponse(QList<QStringList> fieldsLists);
	void createModelResponse(bool modelCreated);
	void addNotesResponse(bool notesCreated);

private:
	QString m_hostName;
	QMetaObject::Connection mGetDeckNamesConnect;
	QMetaObject::Connection mCreateDeckConnect;
	QMetaObject::Connection mGetModelNamesConnect;
	QMetaObject::Connection mGetModelFieldNamesConnect;
	QMetaObject::Connection mGetModelsFieldNamesConnect;
	QMetaObject::Connection mCreateModelConnect;
	QMetaObject::Connection mAddNotesConnect;

private:
	QString FormatRequest(const QString& action, const QJsonObject& params);
	QString FormatRequest(const std::vector<std::pair<QString, QJsonObject>>& actions);
	void Invoke(RequestManager* requestManager, const QString& action, const QJsonObject& params);
	void Invoke(RequestManager* requestManager, const std::vector<std::pair<QString, QJsonObject>>& actions);
private slots:
	void onGetDeckNamesResponse(QString data);
	void onCreateDeckResponse(QString data);
	void onGetModelNamesResponse(QString data);
	void onGetModelFieldNamesResponse(QString data);
	void onGetModelsFieldNamesResponse(QString data);
	void onCreateModelResponse(QString data);
	void onAddNotesResponse(QString data);
};