#include "AnkiConnectLib.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <QFuture>

AnkiConnect::AnkiConnect()
	: QObject(nullptr)
{
	;
}

QString AnkiConnect::FormatRequest(const QString& action, const QJsonObject& params)
{
	QJsonObject request
	{
		{"action", action},
		{ "version", 6 }
	};
	if (!params.isEmpty()) {
		request["params"] = params;
	}

	return QJsonDocument(request).toJson(QJsonDocument::Compact);
}

QString AnkiConnect::FormatRequest(const std::vector<std::pair<QString, QJsonObject>>& actions)
{
	QJsonArray actionsArray;
	for (const auto& [action, params] : actions) {
		actionsArray.append(
			QJsonObject {
				{ "action", action},
				{ "version", 6 },
				{ "params", params }
			}
		);
	}

	QJsonObject request
	{
		{"action", "multi"},
		{ "version", 6 },
		{ "params", QJsonObject{{"actions", actionsArray}} }
	};

	return QJsonDocument(request).toJson(QJsonDocument::Compact);
}

void AnkiConnect::Invoke(RequestManager* requestManager, const QString& action, const QJsonObject& params)
{
	requestManager->POST("http://localhost:8765", FormatRequest(action, params));
}

void AnkiConnect::Invoke(RequestManager* requestManager, const std::vector<std::pair<QString, QJsonObject>>& actions)
{
	requestManager->POST("http://localhost:8765", FormatRequest(actions));
}

/*!
  \fn void AnkiConnect::GetDeckNames(std::function<void(QStringList)>& slot)

  Retrieves a list of decks stored in Anki
  and returns it in the QStringList parameter of \a slot.
*/
void AnkiConnect::GetDeckNames(std::function<void(QStringList)>& slot)
{
	while (mGetDeckNamesConnect) {
		QCoreApplication::processEvents();
	}

	QJsonObject params =
	{
	};

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onGetDeckNamesResponse);
	connect(this, &AnkiConnect::getDeckNamesResponse, slot);

	Invoke(requestManager, "deckNames", params);
}

void AnkiConnect::onGetDeckNamesResponse(QString data)
{
	QStringList decks;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("result") && object["result"].isArray()) {
			QJsonArray deckArray = object["result"].toArray();
			for (auto deckArrayItem : deckArray) {
				if (deckArrayItem.isString()) {
					decks.append(deckArrayItem.toString());
				}
			}
		}
	}

	emit getDeckNamesResponse(decks);
	disconnect(mGetDeckNamesConnect);
}

/*!
  \fn void AnkiConnect::createDeck(QString deckName, std::function<void(bool)>& slot)

  Creates a new deck named \a deckName
  and returns whether the deck was successfully created
  in the bool parameter of \a slot.
*/
void AnkiConnect::createDeck(QString deckName, std::function<void(bool)>& slot)
{
	while (mCreateDeckConnect) {
		QCoreApplication::processEvents();
	}

	QJsonObject params =
	{
		{"deck", deckName}
	};

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onCreateDeckResponse);
	connect(this, &AnkiConnect::createDeckResponse, slot);

	Invoke(requestManager, "createDeck", params);
}

void AnkiConnect::onCreateDeckResponse(QString data)
{
	bool deckCreated = false;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("result") && object["result"].isDouble()) {
			deckCreated = true;
		}
	}

	emit createDeckResponse(deckCreated);
	disconnect(mCreateDeckConnect);
}

/*!
  \fn void AnkiConnect::getModelNames(std::function<void(QStringList)>& slot)

  Retrieves a list of models stored in Anki
  and returns it in the QStringList parameter of \a slot.
*/
void AnkiConnect::getModelNames(std::function<void(QStringList)>& slot)
{
	while (mGetModelNamesConnect) {
		QCoreApplication::processEvents();
	}

	QJsonObject params =
	{
	};

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onGetModelNamesResponse);
	mGetModelNamesConnect = connect(this, &AnkiConnect::getModelNamesResponse, slot);

	Invoke(requestManager, "modelNames", params);
}

void AnkiConnect::onGetModelNamesResponse(QString data)
{
	QStringList models;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("result") && object["result"].isArray()) {
			QJsonArray deckArray = object["result"].toArray();
			for (auto deckArrayItem : deckArray) {
				if (deckArrayItem.isString()) {
					models.append(deckArrayItem.toString());
				}
			}
		}
	}

	emit getModelNamesResponse(models);
	disconnect(mGetModelNamesConnect);
}

/*!
  \fn void AnkiConnect::getModelFieldNames(std::function<void(QStringList)>& slot)

  Retrieves a list of fields for the given \a modelName stored in Anki
  and returns it in the QStringList parameter of \a slot.
*/
void AnkiConnect::getModelFieldNames(QString modelName, std::function<void(QStringList)>& slot)
{
	while (mGetModelFieldNamesConnect) {
		QCoreApplication::processEvents();
	}

	QJsonObject params =
	{
		{"modelName", modelName}
	};

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onGetModelFieldNamesResponse);
	mGetModelFieldNamesConnect = connect(this, &AnkiConnect::getModelFieldNamesResponse, slot);

	Invoke(requestManager, "modelFieldNames", params);
}

void AnkiConnect::onGetModelFieldNamesResponse(QString data)
{
	QStringList fields;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("result") && object["result"].isArray()) {
			QJsonArray deckArray = object["result"].toArray();
			for (auto deckArrayItem : deckArray) {
				if (deckArrayItem.isString()) {
					fields.append(deckArrayItem.toString());
				}
			}
		}
	}

	emit getModelFieldNamesResponse(fields);
	disconnect(mGetModelFieldNamesConnect);
}

/*!
  \fn void AnkiConnect::getModelsFieldNames(std::function<void(QStringList)>& slot)

  Retrieves a list of lists of fields for the given \a modelNames stored in Anki
  and returns it in the QList<QStringList> parameter of \a slot.
*/
void AnkiConnect::getModelsFieldNames(QStringList modelNames, std::function<void(QList<QStringList>)>& slot)
{
	while (mGetModelsFieldNamesConnect) {
		QCoreApplication::processEvents();
	}

	std::vector <std::pair<QString, QJsonObject>> actions;
	for (const auto& modelName : modelNames) {
		actions.push_back({"modelFieldNames", {{"modelName", modelName}}});
	}

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onGetModelsFieldNamesResponse);
	mGetModelsFieldNamesConnect = connect(this, &AnkiConnect::getModelsFieldNamesResponse, slot);

	Invoke(requestManager, actions);
}

void AnkiConnect::onGetModelsFieldNamesResponse(QString data)
{
	QList<QStringList> fieldsLists;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("result") && object["result"].isArray()) {
			QJsonArray resultsArray = object["result"].toArray();
			for (auto resultsArrayItem : resultsArray) {
				QStringList fields;
				if (resultsArrayItem.isObject()) {
					QJsonObject resultObject = resultsArrayItem.toObject();
					if (resultObject.contains("result") && resultObject["result"].isArray()) {
						QJsonArray resultArray = resultObject["result"].toArray();
						for (auto result : resultArray) {
							if (result.isString()) {
								fields.append(result.toString());
							}
						}
					}
				}
				fieldsLists.append(fields);
			}
		}
	}

	emit getModelsFieldNamesResponse(fieldsLists);
	disconnect(mGetModelsFieldNamesConnect);
}

/*!
  \fn void AnkiConnect::createModel(QString modelName, NewCardTypeData newCardTypeData, std::function<void(bool)>& slot)

  Creates a new model named \a modelName with fields in \a newCardTypeData
  and front and back templates in \a newCardTypeData,
  and returns whether the model was successfully created
  in the bool parameter of \a slot.
*/
void AnkiConnect::createModel(QString modelName, NewCardTypeData newCardTypeData, std::function<void(bool)>& slot)
{
	while (mCreateModelConnect) {
		QCoreApplication::processEvents();
	}

	QJsonObject params;
	QJsonArray fieldsArray;
	for (const auto& field : newCardTypeData.fields) {
		fieldsArray.append(field);
	}

	QJsonArray cardTemplatesArray = {
		{QJsonObject({{"Front", newCardTypeData.frontTemplate}, {"Back", newCardTypeData.backTemplate}})}
	};

	params["modelName"] = modelName;
	params["inOrderFields"] = fieldsArray;
	params["cardTemplates"] = cardTemplatesArray;

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onCreateModelResponse);
	mCreateModelConnect = connect(this, &AnkiConnect::createModelResponse, slot);

	Invoke(requestManager, "createModel", params);
}

void AnkiConnect::onCreateModelResponse(QString data)
{
	bool modelCreated = false;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("error") && object["error"].isNull()) {
			modelCreated = true;
		}
	}

	emit createModelResponse(modelCreated);
	disconnect(mCreateModelConnect);
}

/*!
  \fn void AnkiConnect::addNote(QString deckName, QString modelName, std::vector<std::pair<QString, QString>> fields, QStringList tags, std::function<void(bool)>& slot)

  Creates a new note in \a deckName with \a fields from \a modelName and \a tags
  and returns whether the note was successfully created
  in the bool parameter of \a slot.
*/
void AnkiConnect::addNote(QString deckName, QString modelName, std::vector<std::pair<QString, QString>> fields, QStringList tags, std::function<void(bool)>& slot)
{
	addNotes({ { deckName, modelName, fields, tags } }, slot);
}

#include <unordered_set>
void AnkiConnect::addNotes( const std::vector<NoteData>& notesData, std::function<void(bool)>& slot)
{
	while (mAddNotesConnect) {
		QCoreApplication::processEvents();
	}

	QJsonArray notes;
	std::unordered_set<QString> uniqueDecks;
	//std::unordered_map<QString, std::unordered_set<QString>> uniqueNoteTypes;
	for (auto& noteData : notesData) {
		QJsonObject fields;
		for(const auto& [fieldName, fieldContent] : noteData.fields) {
			fields[fieldName] = fieldContent;
			//uniqueNoteTypes[noteData.modelName].insert(fieldName);
		}

		QJsonArray tags;
		for (auto& tag : noteData.tags) {
			tags.append(tag);
		}
		QJsonObject noteObject =
		{
			{"deckName", noteData.deckName},
			{"modelName", noteData.modelName},
			{"fields", fields},
			{"tags", tags}
		};
		uniqueDecks.insert(noteData.deckName);
		notes.append(noteObject);
	}
	
	std::vector<std::pair<QString, QJsonObject>> actions;
	for (const auto& uniqueDeck : uniqueDecks) {
		actions.push_back({ "createDeck", {{"deck", uniqueDeck}} });
	}
	/*for (const auto& [modelName, fieldNames] : uniqueNoteTypes) {
		QJsonArray fieldNamesArray;
		for (const auto& fieldName : fieldNames) {
			fieldNamesArray.append(fieldName);
		}
		actions.push_back({ "createModel",
			{{"modelName", modelName},
			{"inOrderFields", fieldNamesArray},
			{"cardTemplates",QJsonArray()}}
			});
	}*/
	actions.push_back({ "addNotes", {{"notes", notes}} });

	RequestManager* requestManager = new RequestManager;

	connect(requestManager, &RequestManager::sendSignal, this, &AnkiConnect::onAddNotesResponse);
	connect(this, &AnkiConnect::addNotesResponse, slot);

	Invoke(requestManager, actions);
}

void AnkiConnect::onAddNotesResponse(QString data)
{
	bool notesAdded = false;

	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	if (doc.isObject()) {
		QJsonObject object = doc.object();
		if (object.contains("result") && object["result"].isArray()) {
			QJsonArray results = object["result"].toArray();
			for (const auto result : results) {
				if (result.isObject()) {
					QJsonObject addNotesResult = result.toObject();
					if (addNotesResult.contains("error") && addNotesResult["error"].isNull()) {
						notesAdded = true;
						continue;
					}
					notesAdded = false;
					break;
				}
			}
		}
	}

	emit addNotesResponse(notesAdded);
	disconnect(mAddNotesConnect);
}