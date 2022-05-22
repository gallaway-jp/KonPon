from enum import Enum
import tkinter
from tkinter.ttk import Notebook

from language import get_ui_string, StringID as ID
from text_wordlist_gui import TextWordList

class WordListType(Enum):
    ALL = 1 #System
    KNOWN = 2 #English
    UNKNOWN = 3 #Japanese

class WordListsNotebook(Notebook):
    def __init__(self, master: tkinter.Misc, words : dict, \
        known_words : list, unknown_words : list, text_dialog = None) -> None:
        super().__init__(master=master)
        self.words : dict = words
        self.text_dialog = text_dialog
        self.all_known_words : list = known_words
        self.all_unknown_words : list = unknown_words

        self.frm_all : tkinter.Frame = self._add_frame(word_list_type=WordListType.ALL)
        self.frm_known : tkinter.Frame = self._add_frame(word_list_type=WordListType.KNOWN)
        self.frm_unknown : tkinter.Frame = self._add_frame(word_list_type=WordListType.UNKNOWN)

    def _add_frame(self, word_list_type : WordListType) -> tkinter.Frame:
        frame = tkinter.Frame(self)
        frame.pack(fill=tkinter.BOTH, expand=True)

        tv_word_list : WordListType

        match word_list_type:
            case WordListType.ALL:
                tv_word_list = self._add_word_list_all(frame=frame)
            case WordListType.KNOWN:
                tv_word_list = self._add_word_list_known(frame=frame)
            case WordListType.UNKNOWN:
                tv_word_list = self._add_word_list_unknown(frame=frame)
            case _:
                pass

        tv_word_list.pack(fill=tkinter.BOTH, expand=True)

        return frame

    def _add_word_list_all(self, frame : tkinter.Frame) -> TextWordList:
        self.add(frame, text=get_ui_string(ID.TEXT_WORDLIST_ALL_WORDS))
        return TextWordList(master=frame, words=self.words, text_dialog=self.text_dialog)
        
    def _add_word_list_known(self, frame : tkinter.Frame) -> TextWordList:
        self.add(frame, text=get_ui_string(ID.TEXT_WORDLIST_KNOWN_WORDS))
        text_known_words = {k: v for k, v in self.words.items() if k in self.all_known_words}
        return TextWordList(master=frame, words=text_known_words, text_dialog=self.text_dialog)

    def _add_word_list_unknown(self, frame : tkinter.Frame) -> TextWordList:
        self.add(frame, text=get_ui_string(ID.TEXT_WORDLIST_UNKNOWN_WORDS))
        text_unknown_words = {k: v for k, v in self.words.items() if k in self.all_unknown_words}
        return TextWordList(master=frame, words=text_unknown_words, text_dialog=self.text_dialog)


