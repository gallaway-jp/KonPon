#include "Settings.h"

#include "SettingsUtil.h"

#include <QCoreApplication>
#include <QDir>
#include <QGuiApplication>
#include <QIcon>
#include <QPalette>
#include <QSettings>
#include <QString>
#include <QStyleHints>

Settings::Settings() : mFile(), ui(), mParser() {}

Settings::~Settings()
{
	if (m_clearDataOnTermination) {
		QSettings settings;
		settings.clear();
		QDir(mFile.workspace + QString("/KonPonData")).removeRecursively();
	}
}

void Settings::clearDataOnTermination()
{
	m_clearDataOnTermination = true;
}

Settings::File::File()
{
	defaultWorkspace = QCoreApplication::applicationDirPath();

	ReadSettings();
}

Settings::File::~File()
{
	WriteSettings();
}

void Settings::File::setWorkspace(const QString& workspace)
{
	if (workspace != this->workspace) {
		MoveFolderContents(this->workspace, workspace);
		this->workspace = workspace;
	}
}

void Settings::File::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("File");

	workspace = settings.value("workspace", defaultWorkspace).toString();
	settings.endGroup();
}

void Settings::File::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("File");
	settings.setValue("workspace", workspace);
	settings.endGroup();
}

Settings::UI::UI()
{
	m_isSystemDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
	m_systemLanguage = QLocale::system().language() == QLocale::Japanese ? Language::Japanese : Language::English;
	ReadSettings();
}

Settings::UI::~UI()
{
	WriteSettings();
}

void Settings::UI::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("UI");
	m_theme = static_cast<Theme>(settings.value("theme", static_cast<int>(Theme::Default)).toInt());
	if (m_theme != Theme::Default) {
		if ((m_isSystemDarkMode && m_theme != Theme::Dark)
			|| (!m_isSystemDarkMode && m_theme == Theme::Dark)) {
			setPalette(m_theme);
		}
	}
	QGuiApplication::setWindowIcon(QIcon(isDarkTheme() ? ":/res/icons/KonPon_white.ico" : ":/res/icons/KonPon.ico"));

	m_language = static_cast<Language>(settings.value("language", static_cast<int>(Language::Default)).toInt());
	setLanguage(m_language);
	settings.endGroup();
}

void Settings::UI::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("UI");
	settings.setValue("theme", static_cast<int>(m_theme));
	settings.setValue("language", static_cast<int>(m_language));
	settings.endGroup();
}

void Settings::UI::setPalette(Theme theme) {
	struct PaletteItem {
		QPalette::ColorRole role;
		unsigned int activeDarkColor;
		unsigned int disabledDarkColor;
		unsigned int inactiveDarkColor;
		unsigned int activeLightColor;
		unsigned int disabledLightColor;
		unsigned int inactiveLightColor;
	};

	QPalette::ColorRole;
	QPalette palette = QGuiApplication::palette();
	std::vector<PaletteItem> paletteItems = {
		{QPalette::Accent, 0xff0078d7, 0xff0078d7, 0xff0078d7, 0xff0078d7, 0xff787878, 0xfff0f0f0},
		{QPalette::AlternateBase, 0xff002642, 0xf343434, 0xff002642, 0xffe9e7e3, 0xfff7f7f7, 0xffe9e7e3},
		{QPalette::Base, 0xff2d2d2d, 0xff1e1e1e, 0xff2d2d2d, 0xffffffff, 0xfff0f0f0, 0xffffffff},
		{QPalette::BrightText, 0xffa6d8ff, 0xffa6d8ff, 0xffa6d8ff, 0xffffffff, 0xffffffff, 0xffffffff},
		{QPalette::Button, 0xff3c3c3c, 0xff3c3c3c, 0xff3c3c3c, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0},
		{QPalette::ButtonText, 0xffffffff, 0xff9d9d9d, 0xffffffff, 0xff000000, 0xff787878, 0xff000000},
		{QPalette::Dark, 0xff1e1e1e, 0xff1e1e1e, 0xff1e1e1e, 0xffa0a0a0, 0xffa0a0a0, 0xffa0a0a0},
		{QPalette::Highlight, 0xff0078d7, 0xff0078d7, 0xff1e1e1e, 0xff0078d7, 0xff0078d7, 0xfff0f0f0},
		{QPalette::HighlightedText, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff000000},
		{QPalette::Light, 0xff787878, 0xff787878, 0xff787878, 0xffffffff, 0xffffffff, 0xffffffff},
		{QPalette::Link, 0xff0078d7, 0xff308cc6, 0xff0078d7, 0xff0078d7, 0xff0000ff, 0xff0078d7},
		{QPalette::LinkVisited, 0xff002642, 0xffff00ff, 0xff002642, 0xff002642, 0xffff00ff, 0xff002642},
		{QPalette::Mid, 0xff282828, 0xff282828, 0xff282828, 0xffa0a0a0, 0xffa0a0a0, 0xffa0a0a0},
		{QPalette::Midlight, 0xff5a5a5a, 0xff5a5a5a, 0xff5a5a5a, 0xffe3e3e3, 0xfff7f7f7, 0xffe3e3e3},
		{QPalette::NoRole, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000},
		{QPalette::PlaceholderText, 0x80ffffff, 0x80ffffff, 0x80ffffff, 0x80000000, 0x80000000, 0x80000000},
		{QPalette::Shadow, 0xff000000, 0xff000000, 0xff000000, 0xff696969, 0xff000000, 0xff696969},
		{QPalette::Text, 0xffffffff, 0xff9d9d9d, 0xffffffff, 0xff000000, 0xff787878, 0xff000000},
		{QPalette::ToolTipBase, 0xff3c3c3c, 0xffffffdc, 0xff3c3c3c, 0xffffffdc, 0xffffffdc, 0xffffffdc},
		{QPalette::ToolTipText, 0xffd4d4d4, 0xff000000, 0xffd4d4d4, 0xff000000, 0xff000000, 0xff000000},
		{QPalette::Window, 0xff1e1e1e, 0xff1e1e1e, 0xff1e1e1e, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0},
		{QPalette::WindowText, 0xffffffff, 0xff9d9d9d, 0xffffffff, 0xff000000, 0xff787878, 0xff000000},
	};
	for (const auto& item : paletteItems) {
		palette.setColor(QPalette::Active, item.role, theme == Theme::Dark ? item.activeDarkColor : item.activeLightColor);
		palette.setColor(QPalette::Disabled, item.role, theme == Theme::Dark ? item.disabledDarkColor : item.disabledLightColor);
		palette.setColor(QPalette::Inactive, item.role, theme == Theme::Dark ? item.inactiveDarkColor : item.inactiveLightColor);
	}
	QGuiApplication::setPalette(palette);
	QGuiApplication::setWindowIcon(QIcon(theme == Theme::Dark ? ":/res/icons/KonPon_white.ico" : ":/res/icons/KonPon.ico"));
}

Settings::Theme Settings::UI::getTheme()
{
	return m_theme;
}

void Settings::UI::setTheme(Settings::Theme theme)
{
	if (m_theme != theme) {
		switch (theme)
		{
		case Settings::Theme::Default:
			setPalette(m_isSystemDarkMode ? Settings::Theme::Dark : Settings::Theme::Light);
			break;
		case Settings::Theme::Light:
		case Settings::Theme::Dark:
		default:
			setPalette(theme);
			break;
		};
		m_theme = theme;
	}
}

bool Settings::UI::isDarkTheme()
{
	switch (m_theme)
	{
	case Settings::Theme::Default:
		if (m_isSystemDarkMode) {
			return true;
		}
		return false;
	case Settings::Theme::Light:
		return false;
	case Settings::Theme::Dark:
		return true;
	default:
		return false;
	}
}

Settings::Language Settings::UI::getLanguage()
{
	return m_language;
}

void Settings::UI::setLanguage(Settings::Language language)
{
	QCoreApplication::removeTranslator(&m_translator);
	switch (language)
	{
	case Settings::Language::Default:
		switch (m_systemLanguage)
		{
		case Settings::Language::Japanese:
			if (m_translator.load(":/res/translations/Translation_ja.qm")) {
				QCoreApplication::installTranslator(&m_translator);
			}
			break;
		default:
			if (m_translator.load(":/res/translations/Translation_en.qm")) {
				QCoreApplication::installTranslator(&m_translator);
			}
			break;
		}
		break;
	case Settings::Language::English:
		if (m_translator.load(":/res/translations/Translation_en.qm")) {
			QCoreApplication::installTranslator(&m_translator);
		}
		break;
	case Settings::Language::Japanese:
		if (m_translator.load(":/res/translations/Translation_ja.qm")) {
			QCoreApplication::installTranslator(&m_translator);
		}
		break;
	default:
		break;
	}
	m_language = language;
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

void Settings::Anki::setAddress(const QString& address)
{
	this->address = address;
}

void Settings::Anki::setPort(ushort port)
{
	this->port = port;
}

void Settings::Anki::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("Anki");
	isAnkiConnectFeatureEnabled = settings.value("enableAnkiConnectFeature", true).toBool();
	address = settings.value("address", "http://localhost").toString();
	port = static_cast<ushort>(settings.value("port", 8765).toUInt());
	settings.endGroup();
}

void Settings::Anki::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("Anki");
	settings.setValue("enableAnkiConnectFeature", isAnkiConnectFeatureEnabled);
	settings.setValue("address", address);
	settings.setValue("port", port);
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