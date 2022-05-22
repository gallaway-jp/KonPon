from typing import Dict

from lang.string_ids import StringID as ID

ui_strings : Dict[ID, str] = {
    ID.MAIN_TITLE : "KonPon",

    ID.MAIN_MENU_FILE : "File",
    ID.MAIN_MENU_FILE_IMPORT_TEXT : "Import Text",
    ID.MAIN_MENU_FILE_VIEW_WORD_LISTS : "View Word Lists",
    ID.MAIN_MENU_FILE_EXIT : "Exit",

    ID.MAIN_MENU_TOOLS : "Tools",
    ID.MAIN_MENU_TOOLS_SETTINGS : "Settings",

    ID.SETTINGS_TITLE : "Settings",
    ID.SETTINGS_FILE : "File",
    ID.SETTINGS_FILE_CHANGE_WORKSPACE : "Change Workspace",
    ID.SETTINGS_GUI : "GUI",
    ID.SETTINGS_GUI_THEME : "Theme",
    ID.SETTINGS_GUI_LANGUAGE : "Language",

    ID.MAIN_MENU_HELP : "Help",
    ID.MAIN_MENU_HELP_ABOUT : "About",

    ID.MAIN_MENU_HELP_ABOUT_TITLE : "About KonPon",
    ID.MAIN_MENU_HELP_ABOUT_INFO : "Created by Colin\nVer ",

    ID.TEXT_LIST_NAME : "Name",
    ID.TEXT_LIST_DATE : "Date",
    ID.TEXT_LIST_PERCENTAGE : "% Words Known",
    ID.TEXT_LIST_RIGHT_CLICK_COPY_PATH : "Copy File Path to Clipboard",
    ID.TEXT_LIST_RIGHT_CLICK_DELETE : "Delete",

    ID.TEXT_TITLE : "Text",
    ID.TEXT_ACTION_FRAME_ALL_WORDS : "Create Anki Deck from All Words",
    ID.TEXT_ACTION_FRAME_KNOWN_WORDS : "Create Anki Deck from Known Words",
    ID.TEXT_ACTION_FRAME_UNKNOWN_WORDS : "Create Anki Deck from Unknown Words",
    ID.TEXT_WORDLIST_KNOWN_WORDS : "Known Words",
    ID.TEXT_WORDLIST_UNKNOWN_WORDS : "Unknown Words",
    ID.TEXT_WORDLIST_ALL_WORDS : "All",
    
    ID.TEXT_ANKI_PAGE_NOTE_TYPE : "Note Type",
    ID.TEXT_ANKI_PAGE_DECK : "Deck",
    ID.TEXT_ANKI_PAGE_TAGS : "Tags",
    ID.TEXT_ANKI_PAGE_FIELDS_RETRIEVAL_FAILED_1 : "Could not retrieve fields for ",
    ID.TEXT_ANKI_PAGE_FIELDS_RETRIEVAL_FAILED_2 : " note type.",
    ID.TEXT_ANKI_ACTION_FRAME_RETURN : "Return to text",
    ID.TEXT_ANKI_ACTION_FRAME_EXPORT : "Export Deck as Anki File",
    ID.TEXT_ANKI_ACTION_FRAME_SEND : "Send Deck to Anki",

    ID.ACTION_FRAME_IMPORT_TEXT : "Import Text",
    ID.ACTION_FRAME_VIEW_WORD_LISTS : "View Word Lists",
    ID.ACTION_FRAME_VIEW_TEXT : "View Text",
    ID.ACTION_FRAME_DELETE_TEXT : "Delete Text",

    ID.WORD_LISTS_TITLE : "Word Lists",
    ID.WORD_LISTS_UNKNOWN_WORDS : "Unknown Words",
    ID.WORD_LISTS_KNOWN_WORDS : "Known Words",
    ID.WORD_LISTS_ADD : "Add",
    ID.WORD_LISTS_DELETE : "Delete",
    ID.WORD_LISTS_CONFIRM : "Confirm",

    ID.ADD_WORDS_TITLE : "Add Words",
    ID.ADD_WORDS_UNKNOWN_WORDS : "Unknown Words",
    ID.ADD_WORDS_KNOWN_WORDS : "Known Words",
    ID.ADD_WORDS_CONFIRM : "Confirm",
    #
}