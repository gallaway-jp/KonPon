#pragma once

#include <QString>
//class QString;
class Settings {
private:
	class File {
	public:
		File(QString& mAppPath);
		QString workspace;
	private:
		void ReadSettings(QString &appPath);
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
	Settings(QString &appPath);
	File mFile;
	Parser mParser;
};