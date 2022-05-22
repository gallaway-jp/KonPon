from os import name as os_type

from ini import Language, get_settings
from lang.string_ids import StringID
from lang.english import ui_strings as en_strings
from lang.japanese import ui_strings as ja_strings

DEFAULT_LANGUAGE = Language.DEFAULT
CURRENT_LANGUAGE = get_settings("GUI", "Language")

def get_default_language() -> Language:
    global DEFAULT_LANGUAGE
    if DEFAULT_LANGUAGE == Language.DEFAULT:
        DEFAULT_LANGUAGE = update_default_language()
    return DEFAULT_LANGUAGE

def get_current_language() -> Language:
    global CURRENT_LANGUAGE
    if CURRENT_LANGUAGE == Language.DEFAULT:
        CURRENT_LANGUAGE = get_default_language()
    return CURRENT_LANGUAGE

def set_current_language(lang : Language) -> None:
    global CURRENT_LANGUAGE
    CURRENT_LANGUAGE = lang

def update_default_language() -> Language:
    language = ""
    match os_type:
        case "nt": #Windows
            from locale import windows_locale
            from ctypes import windll
            lang_id = windll.kernel32.GetUserDefaultUILanguage()
            language = windows_locale[lang_id]
        case _ : #OS other than Windows
            from locale import getdefaultlocale
            language, encoding = getdefaultlocale()
    if language and len(language) >= 2:
        match language[0:2]:
            case "en":
                return Language.EN
            case "ja":
                return Language.JA
            case _:
                return Language.EN

def get_ui_string(ui_location : StringID) -> str:
    """Returns string in the appropriate language."""
    lang : Language = get_current_language()
    match lang:
        case Language.EN:
            return en_strings[ui_location]
        case Language.JA:
            return ja_strings[ui_location]
        case _:
            return en_strings[ui_location]