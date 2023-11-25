#pragma once

#include "settingslib_global.h"

#include <QString>

class SETTINGSLIB_EXPORT Settings {
private:
	class File {
	public:
		File(QString& mAppPath);
		QString workspace;
	private:
		void ReadSettings(QString& appPath);
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
};