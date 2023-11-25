#pragma once
#include <QString.h>
#include <QJsonObject.h>

class JSONParser
{
public:
	JSONParser(QString path);
	~JSONParser();
private:
	bool loadData(QString path);
	bool parseJSON(const QJsonObject& json);
};

