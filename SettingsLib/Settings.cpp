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

/*!
	\fn Settings::Settings()

	Constructs a Settings object and initializes its mFile, ui, and mParser members.
*/
Settings::Settings() : mFile(), ui(), mParser() {}

/*!
	\fn Settings::~Settings()

	Will clear KonPon data on destruction of Settings object if
	Clear Data button had been pressed.
*/
Settings::~Settings()
{
	if (m_clearDataOnTermination) {
		QSettings settings;
		settings.clear();
#ifdef Q_OS_WIN
		QSettings windowsSettings = QSettings("HKEY_CURRENT_USER\\Software\\" + QCoreApplication::organizationName(),
			QSettings::NativeFormat);
		if (windowsSettings.childKeys().isEmpty()) {
			windowsSettings.remove("");
		}
#endif // Q_OS_WIN
		QDir(mFile.workspace + QString("/KonPonData")).removeRecursively();
	}
}

/*!
	\fn void Settings::clearDataOnTermination()

	Sets the m_clearDataOnTermination flag to true.
	KonPon data will be cleared upon destruction of Settings object.
*/
void Settings::clearDataOnTermination()
{
	m_clearDataOnTermination = true;
	mAnki.clearDataOnTermination();
	mFile.clearDataOnTermination();
	ui.clearDataOnTermination();
}

/*!
	\fn Settings::File::File()

	Constructs a File object that manages File-related settings
	and reads settings from storage.
*/
Settings::File::File()
{
	defaultWorkspace = QCoreApplication::applicationDirPath();

	ReadSettings();
}

/*!
	\fn Settings::File::~File()

	Writes settings to storage upon destruction of File object.
*/
Settings::File::~File()
{
	if (!m_clearDataOnTermination) {
		WriteSettings();
	}
}

/*!
	\fn void Settings::File::clearDataOnTermination()

	Sets the m_clearDataOnTermination flag to true.
	KonPon data will be cleared upon destruction of Settings object.
*/
void Settings::File::clearDataOnTermination()
{
	m_clearDataOnTermination = true;
}

/*!
	\fn void Settings::File::setWorkspace(const QString& workspace)

	Sets workspace settings to \a workspace and moves KonPon data
	from the old workspace to the new workspace.
*/
void Settings::File::setWorkspace(const QString& workspace)
{
	if (workspace != this->workspace) {
		MoveFolderContents(this->workspace + QString("/KonPonData"), workspace + QString("/KonPonData"));
		this->workspace = workspace;
	}
}

/*!
	\fn void Settings::File::ReadSettings()

	Reads the File-related settings from storage.
*/
void Settings::File::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("File");

	workspace = settings.value("workspace", defaultWorkspace).toString();
	settings.endGroup();
}

/*!
	\fn void Settings::File::WriteSettings()

	Reads the File-related settings from storage.
*/
void Settings::File::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("File");
	settings.setValue("workspace", workspace);
	settings.endGroup();
}

/*!
	\fn Settings::UI::UI()

	Constructs a UI object that manages UI-related settings
	and reads settings from storage.
*/
Settings::UI::UI()
{
	m_isSystemDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
	m_systemLanguage = QLocale::system().language() == QLocale::Japanese ? Language::Japanese : Language::English;
	ReadSettings();
}

/*!
	\fn Settings::UI::~UI()

	Writes settings to storage upon destruction of UI object.
*/
Settings::UI::~UI()
{
	if (!m_clearDataOnTermination) {
		WriteSettings();
	}
}

/*!
	\fn void Settings::UI::clearDataOnTermination()

	Sets the m_clearDataOnTermination flag to true.
	KonPon data will be cleared upon destruction of Settings object.
*/
void Settings::UI::clearDataOnTermination()
{
	m_clearDataOnTermination = true;
}

/*!
	\fn void Settings::UI::ReadSettings()

	Reads the UI-related settings from storage.
*/
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

/*!
	\fn void Settings::UI::WriteSettings()

	Reads the UI-related settings from storage.
*/
void Settings::UI::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("UI");
	settings.setValue("theme", static_cast<int>(m_theme));
	settings.setValue("language", static_cast<int>(m_language));
	settings.endGroup();
}

/*!
	\fn void Settings::UI::setPalette(Theme theme)

	Updates the UI's palette to be \a theme.
	Also sets the window icon based on \a theme.
*/
void Settings::UI::setPalette(Theme theme)
{
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

/*!
	\fn Settings::Theme Settings::UI::getTheme()

	Returns the current theme.
*/
Settings::Theme Settings::UI::getTheme()
{
	return m_theme;
}

/*!
	\fn void Settings::UI::setTheme(Settings::Theme theme)

	Sets the current theme to \a theme and updates the current palette.
*/
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

/*!
	\fn bool Settings::UI::isDarkTheme()

	Returns true if current theme is dark, otherwise returns false.
*/
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

/*!
	\fn Settings::Language Settings::UI::getLanguage()

	Returns the current language.
*/
Settings::Language Settings::UI::getLanguage()
{
	return m_language;
}

/*!
	\fn void Settings::UI::setLanguage(Settings::Language language)

	Sets the current language to \a language and updates the UI.
*/
void Settings::UI::setLanguage(Settings::Language language)
{
	QCoreApplication::removeTranslator(&m_translator);
	QCoreApplication::removeTranslator(&m_translator2);
	switch (language)
	{
	case Settings::Language::Default:
		switch (m_systemLanguage)
		{
		case Settings::Language::Japanese:
			if (m_translator.load(":/res/translations/Translation_ja.qm")) {
				QCoreApplication::installTranslator(&m_translator);
			}
			if (m_translator2.load(":/res/translations/qt_ja.qm")) {
				QCoreApplication::installTranslator(&m_translator2);
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
		if (m_translator2.load(":/res/translations/qt_ja.qm")) {
			QCoreApplication::installTranslator(&m_translator2);
		}
		break;
	default:
		break;
	}
	m_language = language;
}

/*!
	\fn Settings::Anki::Anki()

	Constructs an Anki object that manages Anki-related settings
	and reads settings from storage.
*/
Settings::Anki::Anki()
{
	ReadSettings();
}

/*!
	\fn Settings::Anki::~Anki()

	Writes settings to storage upon destruction of Anki object.
*/
Settings::Anki::~Anki()
{
	if (!m_clearDataOnTermination) {
		WriteSettings();
	}
}

/*!
	\fn void Settings::Anki::clearDataOnTermination()

	Sets the m_clearDataOnTermination flag to true.
	KonPon data will be cleared upon destruction of Settings object.
*/
void Settings::Anki::clearDataOnTermination()
{
	m_clearDataOnTermination = true;
}

/*!
	\fn void Settings::Anki::setEnableAnkiConnectFeature(bool enable)

	Sets the isAnkiConnectFeatureEnabled setting to \a enable.
*/
void Settings::Anki::setEnableAnkiConnectFeature(bool enable)
{
	isAnkiConnectFeatureEnabled = enable;
}

/*!
	\fn void Settings::Anki::setAddress(const QString& address)

	Sets the address setting to \a address.
*/
void Settings::Anki::setAddress(const QString& address)
{
	this->address = address;
}

/*!
	\fn void Settings::Anki::setPort(ushort port)

	Sets the port setting to \a port.
*/
void Settings::Anki::setPort(ushort port)
{
	this->port = port;
}

/*!
	\fn void Settings::Anki::ReadSettings()

	Reads the Anki-related settings from storage.
*/
void Settings::Anki::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("Anki");
	isAnkiConnectFeatureEnabled = settings.value("enableAnkiConnectFeature", true).toBool();
	address = settings.value("address", "http://localhost").toString();
	port = static_cast<ushort>(settings.value("port", 8765).toUInt());
	settings.endGroup();
}

/*!
	\fn void Settings::Anki::WriteSettings()

	Reads the Anki-related settings from storage.
*/
void Settings::Anki::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("Anki");
	settings.setValue("enableAnkiConnectFeature", isAnkiConnectFeatureEnabled);
	settings.setValue("address", address);
	settings.setValue("port", port);
	settings.endGroup();
}

/*!
	\fn Settings::Parser::Parser()

	Constructs a Parser object that manages Parser-related settings
	and reads settings from storage.
*/
Settings::Parser::Parser()
{
	ReadSettings();
}

/*!
	\fn void Settings::Parser::ReadSettings()

	Reads the Parser-related settings from storage.
*/
void Settings::Parser::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("Parser");
	settings.endGroup();
}

/*!
	\fn void Settings::Parser::WriteSettings()

	Reads the Parser-related settings from storage.
*/
void Settings::Parser::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("Parser");
	settings.endGroup();
}