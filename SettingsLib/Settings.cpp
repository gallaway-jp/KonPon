#include "Settings.h"

#include <QSettings>
#include <QString>

Settings::Settings(QString& appPath) : mFile(appPath), mParser() {}

Settings::File::File(QString& appPath)
{
	ReadSettings(appPath);
}

void Settings::File::ReadSettings(QString& appPath)
{
	QSettings settings;
	settings.beginGroup("File");
	workspace = settings.value("workspace", appPath).toString();
	settings.endGroup();
}

void Settings::File::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("File");
	settings.setValue("workspace", workspace);
	settings.endGroup();
}

Settings::Parser::Parser()
{
	ReadSettings();
}

void Settings::Parser::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("Parser");
	settings.endGroup();
}

void Settings::Parser::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("Parser");
	settings.endGroup();
}