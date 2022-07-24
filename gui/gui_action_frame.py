from pathlib import Path
import tkinter
from tkinter.ttk import Button, Frame, Separator
from language import get_ui_string, StringID as ID

from tokenizer import open_and_tokenize_texts
from wordlist_gui import Wordlist

class ActionFrame():
    def __init__(self, parent : tkinter.Misc, textlist, known_words : list, unknown_words : list):
        self.parent = parent
        self.textlist = textlist
        self.known_words = known_words
        self.unknown_words = unknown_words

        self._add_widgets()
    
    def _add_widgets(self):
        frame_action = Frame(master=self.parent)

        frame_action.columnconfigure(index=0, weight=1)
        frame_action.rowconfigure(index=0, weight=1)
        frame_action.columnconfigure(index=1, weight=1)
        frame_action.rowconfigure(index=1, weight=1)
        frame_action.columnconfigure(index=2, weight=1)
        frame_action.rowconfigure(index=2, weight=1)
        frame_action.columnconfigure(index=3, weight=1)
        frame_action.rowconfigure(index=3, weight=1)

        self._add_list_action_separator(frame_action)

        btn_import = Button(master=frame_action,
            text=get_ui_string(ID.ACTION_FRAME_IMPORT_TEXT), command=self.handle_import_btn_clicked)
        btn_import.grid(row=1, column=0, padx=5, pady=5, sticky=tkinter.EW)

        btn_edit = Button(master=frame_action,
            text=get_ui_string(ID.ACTION_FRAME_VIEW_WORD_LISTS), command=self.handle_edit_btn_clicked)
        btn_edit.grid(row=1, column=1, padx=5, pady=5, sticky=tkinter.EW)

        btn_view = Button(master=frame_action, text=get_ui_string(ID.ACTION_FRAME_VIEW_TEXT))
        btn_view.grid(row=1, column=2, padx=5, pady=5, sticky=tkinter.EW)

        btn_delete = Button(master=frame_action,
            text=get_ui_string(ID.ACTION_FRAME_DELETE_TEXT), command=self.handle_delete_btn_clicked)
        btn_delete.grid(row=1, column=3, padx=5, pady=5, sticky=tkinter.EW)

        frame_action.pack(fill=tkinter.BOTH, anchor=tkinter.S, side=tkinter.BOTTOM)

    def handle_import_btn_clicked(self) -> None:
        tokenize_results = open_and_tokenize_texts()
        if not tokenize_results:
            return
        for name, date, output_path, word_dictionary in tokenize_results:
            self.textlist.load_new_item(str(Path(name).with_suffix("")), date, output_path, word_dictionary)
    
    def handle_edit_btn_clicked(self):
        """
        Handles the edit button clicked action by creating the Wordlist window.
        """
        Wordlist(known_words=self.known_words, unknown_words=self.unknown_words)

    def handle_delete_btn_clicked(self):
        """
        Handles the deleted button clicked action by calling textlist's delete method, 
        which deletes selected items from the list.
        """
        self.textlist.delete()
        
    def _add_list_action_separator(self, frame) -> None:
        separator = Separator(master=frame, orient=tkinter.HORIZONTAL)
        separator.grid(row=0, column=0, columnspan=4, padx=0, pady=0, sticky=tkinter.EW)