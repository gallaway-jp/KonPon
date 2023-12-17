#include "Words.h"

#include "Word.h"

#include <QDirIterator>
#include <QtConcurrent>

/*!
	\fn QString Words::getPath()

	Formats an AnkiConnect request from one \a action with its \a params.
	Returns a QString of the formatted request.
*/
QString Words::getPath()
{
	QDir dir = QDir(mWorkspace.c_str() + QString("/KonPonData") +
		QString("/Words")
	);
	if (!dir.exists()) {
		// Create folder if does not exist
		dir.mkpath(".");
	}
	return dir.absolutePath();
}

/*!
	\fn bool Words::insertWord(const Word& newWord)

	Inserts \a newWord into Words object's map.
*/
bool Words::insertWord(const Word& newWord)
{	
	auto iter = mWords.find({ newWord.getKana(), newWord.getKanji() });
	if (iter == mWords.end()) {
		mWords[{ newWord.getKana(), newWord.getKanji() }] = newWord;
		return true;
	}
	for (uint8_t newPitchAccent : newWord.getPitchAccents()) {
		mWords[{newWord.getKana(), newWord.getKanji()}].addPitchAccent(newPitchAccent);
	}
	for (uint64_t newPitchAccent : newWord.getTextIds()) {
		mWords[{newWord.getKana(), newWord.getKanji()}].addTextId(newPitchAccent);
	}
	return true;
}

/*!
	\fn bool Words::setWorkspace(const std::string& workspace)

	Sets the workspace of each word to \a workspace.
*/
bool Words::setWorkspace(const std::string& workspace)
{
	for (auto [key, val] : mWords) {
		val.setWorkspace(workspace);
	}
	mWorkspace = workspace;
	return true;
}

/*!
	\fn void Words::removeTextId(int64_t textId)

	Removes \a textId data from each word.
*/
void Words::removeTextId(int64_t textId)
{
	std::ignore = QtConcurrent::run([this, textId]()
		{
			QDirIterator it(getPath(), { "*.json" }, QDir::Files, QDirIterator::Subdirectories);
			while (it.hasNext()) {
				QString wordInfoFilePath = it.next();
				QFile file(wordInfoFilePath);

				if (!file.open(QIODevice::ReadWrite)) {
					continue;
				}
				QByteArray data = file.readAll();

				QJsonDocument doc = QJsonDocument::fromJson(data);
				if (doc.isNull()) {
					file.close();
					continue;
				}

				if (!doc.isObject()) {
					file.close();
					continue;
				}

				QJsonObject jsonObject = doc.object();
				if (!jsonObject.contains("textIds")) {
					file.close();
					continue;
				}
				QJsonValue textIdsValue = jsonObject.value("textIds");
				if (!textIdsValue.isArray()) {
					file.close();
					continue;
				}
				bool updateFile = false;
				QJsonArray textIdsArray = textIdsValue.toArray();
				for (qsizetype i = 0; i < textIdsArray.count(); ++i) {
					if (textIdsArray[i] == textId) {
						textIdsArray.removeAt(i);
						updateFile = true;
						break;
					}
				}
				if (updateFile) {
					jsonObject["textIds"] = textIdsArray;
					file.resize(0);

#ifdef QT_DEBUG
					QJsonDocument::JsonFormat format = QJsonDocument::Indented;
#else
					QJsonDocument::JsonFormat format = QJsonDocument::Compact;
#endif

					file.write(QJsonDocument(jsonObject).toJson(format));
				}
				file.close();
			}
		});
}