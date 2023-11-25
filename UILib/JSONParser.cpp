#include "JSONParser.h"

#include <QFile>
#include <QByteArray>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>



JSONParser::JSONParser(QString path)
{
	;
}

bool JSONParser::loadData(QString path)
{
    QFile loadFile(path);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    parseJSON(loadDoc.object());
    return true;
}

bool JSONParser::parseJSON(const QJsonObject& json)
{
    /*if (const QJsonValue v = json["levels"]; v.isArray()) {
        const QJsonArray levels = v.toArray();
        mLevels.clear();
        mLevels.reserve(levels.size());
        for (const QJsonValue& level : levels)
            mLevels.append(Level::fromJson(level.toObject()));
    }*/
    return true;
}

JSONParser::~JSONParser()
{

}