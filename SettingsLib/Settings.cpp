#include "Settings.h"

#include <QSettings>
#include <QString>

Settings::Settings(QString& appPath) : mFile(appPath), mParser() {}

Settings::File::File(const QString& appPath)
{
	ReadSettings(appPath);
}

Settings::File::~File()
{
	WriteSettings();
}

void Settings::File::setWorkspace(const QString& workspace)
{
	this->workspace = workspace;
}

void Settings::File::ReadSettings(const QString& appPath)
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

Settings::Anki::Anki()
{
	ReadSettings();
}

Settings::Anki::~Anki()
{
	WriteSettings();
}

void Settings::Anki::setEnableAnkiConnectFeature(bool enable)
{
	isAnkiConnectFeatureEnabled = enable;
}

void Settings::Anki::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("Anki");
	isAnkiConnectFeatureEnabled = settings.value("enableAnkiConnectFeature", true).toBool();
	settings.endGroup();
}

void Settings::Anki::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("Anki");
	settings.setValue("enableAnkiConnectFeature", isAnkiConnectFeatureEnabled);
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