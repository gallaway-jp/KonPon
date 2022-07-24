import tkinter
from tkinter.ttk import Frame, Separator, Button

from language import get_ui_string, StringID as ID

class TextActionFrame(Frame):
    def __init__(self, parent : tkinter.Misc, controller: tkinter.Misc, textlist : dict):
        super().__init__(master=parent)

        self.parent = parent
        self.controller = controller
        self.textlist = textlist

        self._add_widgets()
    
    def _add_widgets(self):
        
        self.columnconfigure(index=tuple(range(3)), weight=1)
        self.rowconfigure(index=tuple(range(3)), weight=1)

        self._add_list_action_separator()

        create_all_deck_button = Button(master=self, text=get_ui_string(ID.TEXT_ACTION_FRAME_ALL_WORDS), command=self.on_all_button_clicked)
        create_all_deck_button.grid(row=1, column=0, padx=5, pady=5, sticky=tkinter.EW)

        create_known_deck_button = Button(master=self, text=get_ui_string(ID.TEXT_ACTION_FRAME_KNOWN_WORDS), command=self.on_known_button_clicked)
        create_known_deck_button.grid(row=1, column=1, padx=5, pady=5, sticky=tkinter.EW)

        create_unknown_deck_button = Button(master=self, text=get_ui_string(ID.TEXT_ACTION_FRAME_UNKNOWN_WORDS), command=self.on_unknown_button_clicked)
        create_unknown_deck_button.grid(row=1, column=2, padx=5, pady=5, sticky=tkinter.EW)

        self.grid(row=6, column=0, padx=5, pady=5, rowspan=6, columnspan=7, sticky=tkinter.NSEW)
        
    def _add_list_action_separator(self) -> None:
        separator = Separator(master=self, orient=tkinter.HORIZONTAL)
        separator.grid(row=0, column=0, columnspan=4, padx=0, pady=0, sticky=tkinter.EW)
    
    def on_all_button_clicked(self) -> None:
        self.controller.show_frame("AnkiPage", None)
    
    def on_known_button_clicked(self) -> None:
        self.controller.show_frame("AnkiPage", True)
    
    def on_unknown_button_clicked(self) -> None:
        self.controller.show_frame("AnkiPage", False)