import tkinter
from tkinter.ttk import Frame, Separator, Button

from language import get_ui_string, StringID as ID
from util.threading import work_in_background

class AnkiActionFrame(Frame):
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

        return_to_text_button = Button(master=self,
            text=get_ui_string(ID.TEXT_ANKI_ACTION_FRAME_RETURN), command=self.on_return_button_clicked)
        return_to_text_button.grid(row=1, column=0, padx=5, pady=5, sticky=tkinter.EW)

        export_anki_file_button = Button(master=self,
            text=get_ui_string(ID.TEXT_ANKI_ACTION_FRAME_EXPORT), command=self.on_export_button_clicked)
        export_anki_file_button.grid(row=1, column=1, padx=5, pady=5, sticky=tkinter.EW)

        send_to_anki_button = Button(master=self,
            text=get_ui_string(ID.TEXT_ANKI_ACTION_FRAME_SEND), command=self.on_send_button_clicked)
        send_to_anki_button.grid(row=1, column=2, padx=5, pady=5, sticky=tkinter.EW)

        self.grid(row=6, column=0, padx=5, pady=5, rowspan=6, columnspan=7, sticky=tkinter.NSEW)
        
    def _add_list_action_separator(self) -> None:
        separator = Separator(master=self, orient=tkinter.HORIZONTAL)
        separator.grid(row=0, column=0, columnspan=4, padx=0, pady=0, sticky=tkinter.EW)
    
    def on_return_button_clicked(self) -> None:
        self.controller.show_frame("StartPage")
    
    def on_export_button_clicked(self) -> None:
        self.controller.show_frame("AnkiPage")
    
    def on_send_button_clicked(self) -> None:
        work_in_background(self.parent.send_to_anki())