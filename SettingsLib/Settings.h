#pragma once

#include "settingslib_global.h"

#include <QString>
#include <QTranslator>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

/*!
	\class Settings

	\brief The Settings class provides methods
	to load and save KonPon settings.
*/
class SETTINGSLIB_EXPORT Settings {
public:
	enum class Theme {
		Default,
		Light,
		Dark
	};
	enum class Language {
		Default,
		English,
		Japanese
	};
private:
	bool m_clearDataOnTermination = false;

	class File {
	public:
		File();
		~File();
		void clearDataOnTermination();
		void setWorkspace(const QString& workspace);
		QString workspace;
		QString defaultWorkspace;
	private:
		bool m_clearDataOnTermination = false;
	private:
		void ReadSettings();
		void WriteSettings();
	};

	class UI {
	public:
		UI();
		~UI();
		void clearDataOnTermination();
		void setTheme(Settings::Theme theme);
		Settings::Theme getTheme();
		bool isDarkTheme();
		void setLanguage(Settings::Language language);
		Settings::Language getLanguage();
	private:
		bool m_clearDataOnTermination = false;
		bool m_isSystemDarkMode = false;
		Settings::Theme m_theme = Theme::Default;

		QTranslator m_translator;
		QTranslator m_translator2;
		Settings::Language m_systemLanguage;
		Settings::Language m_language = Language::Default;
	private:
		void setPalette(Settings::Theme theme);
		void ReadSettings();
		void WriteSettings();
	};

	class Anki {
	public:
		Anki();
		~Anki();
		void clearDataOnTermination();
		void setEnableAnkiConnectFeature(bool enable);
		void setAddress(const QString& address);
		void setPort(ushort port);
		bool isAnkiConnectFeatureEnabled = true;
		QString address;
		ushort port = 8765;
	private:
		bool m_clearDataOnTermination = false;
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