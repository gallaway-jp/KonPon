#pragma once

#include "settingslib_global.h"

#include <QString>

class SETTINGSLIB_EXPORT Settings {
private:
	class File {
	public:
		File(const QString& appPath);
		~File();
		void setWorkspace(const QString& workspace);
		QString workspace;
	private:
		void ReadSettings(const QString& appPath);
		void WriteSettings();
	};

	class Anki {
	public:
		Anki();
		~Anki();
		void setEnableAnkiConnectFeature(bool enable);
		bool isAnkiConnectFeatureEnabled = true;
	private:
		void ReadSettings();
		void WriteSettings();
	};

	class Parser {
	public:
		Parser();
	private:
		void ReadSettings();
		void WriteSettings();
	};
public:
	Settings(QString& appPath);
	File mFile;
	Parser mParser;
	Anki mAnki;
};