#pragma once

#include "settingslib_global.h"

#include <QString>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class SETTINGSLIB_EXPORT Settings {
public:
	enum Theme {
		Default,
		Light,
		Dark
	};
private:
	bool m_clearDataOnTermination = false;

	class File {
	public:
		File();
		~File();
		void setWorkspace(const QString& workspace);
		QString workspace;
	private:
		void ReadSettings();
		void WriteSettings();
	};

	class UI {
	public:
		UI();
		~UI();
		void setTheme(Settings::Theme theme);
		Settings::Theme getTheme();
		bool isDarkTheme();
	private:
		bool m_isSystemDarkMode = false;
		Settings::Theme m_theme = Default;
	private:
		void setPalette(Settings::Theme theme);
		void ReadSettings();
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
	Settings();
	~Settings();
	void clearDataOnTermination();
	File mFile;
	UI ui;
	Parser mParser;
	Anki mAnki;
};