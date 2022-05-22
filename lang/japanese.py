from typing import Dict

from lang.string_ids import StringID as ID

ui_strings : Dict[ID, str] = {
    ID.MAIN_TITLE : "根本",

    ID.MAIN_MENU_FILE : "ファイル",
    ID.MAIN_MENU_FILE_IMPORT_TEXT : "テキストを読み込む",
    ID.MAIN_MENU_FILE_VIEW_WORD_LISTS : "言葉一覧を表示",
    ID.MAIN_MENU_FILE_EXIT : "終了",

    ID.MAIN_MENU_TOOLS : "ツール",
    ID.MAIN_MENU_TOOLS_SETTINGS : "設定",

    ID.SETTINGS_TITLE : "設定",
    ID.SETTINGS_FILE : "ファイル",
    ID.SETTINGS_FILE_CHANGE_WORKSPACE : "ワークスペースを変更",
    ID.SETTINGS_GUI : "GUI",
    ID.SETTINGS_GUI_THEME : "テーマ",
    ID.SETTINGS_GUI_LANGUAGE : "言語",

    ID.MAIN_MENU_HELP : "ヘルプ",
    ID.MAIN_MENU_HELP_ABOUT : "アプリについて",

    ID.MAIN_MENU_HELP_ABOUT_TITLE : "根本について",
    ID.MAIN_MENU_HELP_ABOUT_INFO : "Colinにより制作されました\nバージョン　",

    ID.TEXT_LIST_NAME : "テキスト名",
    ID.TEXT_LIST_DATE : "日付",
    ID.TEXT_LIST_PERCENTAGE : "既知語の割合",
    ID.TEXT_LIST_RIGHT_CLICK_COPY_PATH : "クリップボードにファイルパスをコピー",
    ID.TEXT_LIST_RIGHT_CLICK_DELETE : "削除",

    ID.TEXT_TITLE : "テキスト",
    ID.TEXT_ACTION_FRAME_ALL_WORDS : "全ての言葉からAnkiデッキを作成",
    ID.TEXT_ACTION_FRAME_KNOWN_WORDS : "既知語からAnkiデッキを作成",
    ID.TEXT_ACTION_FRAME_UNKNOWN_WORDS : "未知語からAnkiデッキを作成",
    ID.TEXT_WORDLIST_KNOWN_WORDS : "既知語",
    ID.TEXT_WORDLIST_UNKNOWN_WORDS : "未知語",
    ID.TEXT_WORDLIST_ALL_WORDS : "全て",
    
    ID.TEXT_ANKI_PAGE_NOTE_TYPE : "ノートタイプ",
    ID.TEXT_ANKI_PAGE_DECK : "デッキ",
    ID.TEXT_ANKI_PAGE_TAGS : "タグ",
    ID.TEXT_ANKI_PAGE_FIELDS_RETRIEVAL_FAILED_1 : "",
    ID.TEXT_ANKI_PAGE_FIELDS_RETRIEVAL_FAILED_2 : "というノートタイプのフィールドが取得できませんでした。",
    ID.TEXT_ANKI_ACTION_FRAME_RETURN : "テキストに戻る",
    ID.TEXT_ANKI_ACTION_FRAME_EXPORT : "Ankiファイルとしてデッキを出力",
    ID.TEXT_ANKI_ACTION_FRAME_SEND : "Ankiに転送",

    ID.ACTION_FRAME_IMPORT_TEXT : "テキストを\n読み込む",
    ID.ACTION_FRAME_VIEW_WORD_LISTS : "言葉一覧を\n表示",
    ID.ACTION_FRAME_VIEW_TEXT : "テキストを\n表示",
    ID.ACTION_FRAME_DELETE_TEXT : "テキストを\n削除",

    ID.WORD_LISTS_TITLE : "言葉一覧",
    ID.WORD_LISTS_UNKNOWN_WORDS : "未知語",
    ID.WORD_LISTS_KNOWN_WORDS : "既知語",
    ID.WORD_LISTS_ADD : "追加",
    ID.WORD_LISTS_DELETE : "削除",
    ID.WORD_LISTS_CONFIRM : "確認",

    ID.ADD_WORDS_TITLE : "言葉の追加",
    ID.ADD_WORDS_UNKNOWN_WORDS : "未知語",
    ID.ADD_WORDS_KNOWN_WORDS : "既知語",
    ID.ADD_WORDS_CONFIRM : "確認",
    #
}