#include "TextParser.h"
#include "TextParserFunc.h"
#include "MeCab.h"
#include "MecabSlim.h"

#include "Wordlist.h"
#include "Words.h"

#include <string>
#include <map>
#include <chrono>
#include <filesystem>

#include <QString>
#include <QDir>
#include <QByteArray>
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonArray>

bool outputToJsonFile(const char* path, const WordDict &wordDictionary, const LocationsDict& locationsDictionary)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly)) {
		return false;
	}

	QJsonObject object;
	{
		QJsonObject kanaObject;
		for (auto const& [key, val] : wordDictionary)
		{
			QJsonObject kanjiObject;
			for (auto const& [kkey, kval] : val) {
				QJsonObject locationsObject;
				for (auto const& [lkey, lval] : kval) {
					locationsObject[QString::number(lkey)] = QJsonValue(lval);
				}

				kanjiObject[QString(kkey.c_str())] = locationsObject;
			}
			kanaObject[QString(key.c_str())] = kanjiObject;
		}
		object["words"] = kanaObject;
	}
	{
		QJsonObject locationsObject;
		for (auto const& [key, val] : locationsDictionary)
		{
			QJsonObject wordObject;
			for (auto const& [kkey, kval] : val) {
				QJsonArray kanjiArray;
				//qjarr.fromVariantList
				for (const std::string& kanji : kval) {
					kanjiArray.append(kanji.c_str());
				}
				wordObject[kkey.c_str()] = kanjiArray;
			}
			locationsObject[std::to_string(key).c_str()] = wordObject;
		}
		object["locations"] = locationsObject;
	}

	bool writeSucceeded = file.write(QJsonDocument(object).toJson()) > 0;
	file.close();
	return writeSucceeded;
}

int64_t Parser::TokenizeText(std::string const& folder, std::string const& name, const char *pInputText,
	const std::string& fileId, Wordlist& outWordlist, Words& outWords)
{
	auto [wordDictionary, locationsDictionary] = Tokenize(folder, pInputText, fileId, outWordlist, outWords);
	if (wordDictionary.empty()) {
		return 0;
	}

	std::filesystem::path path = std::filesystem::path(folder + "/KonPonData" + "/Texts");
	path /= fileId;
	std::filesystem::create_directories(path);

	std::filesystem::path outputPathText = path;
	outputPathText /= "text.txt";
	std::filesystem::path outputPathCsv = path;
	outputPathCsv /= "wordlist.json";

	if (!outputToJsonFile(outputPathCsv.string().c_str(), wordDictionary, locationsDictionary)) {
		QDir(path).removeRecursively();
		return 0;
	}
	if (!QFile::copy(name.c_str(), outputPathText.string().c_str())) {
		QDir(path).removeRecursively();
		return 0;
	}
	return std::stoll(fileId);
}