import tkinter
from tkinter.ttk import Frame, Label, Combobox, Checkbutton, Notebook

from language import Language, get_ui_string, StringID as ID
from theme import Theme

class AnkiTab(Frame):
    def __init__(self, notebook : Notebook) -> None:
        super().__init__(master=notebook)

        self._configure_rows_cols()

        self._add_widgets_to_tab()

        self.pack(fill=tkinter.BOTH, expand=True)

        notebook.add(child=self, text=get_ui_string(ID.SETTINGS_ANKI))

    def _configure_rows_cols(self) -> None:
        self.columnconfigure(index=tuple(range(2)), weight=1)
        self.rowconfigure(index=tuple(range(2)), weight=1)

    def _add_widgets_to_tab(self) -> None:
        self._add_sync_settings_to_tab()

    def _add_sync_settings_to_tab(self):
        sync_checkbox_value = False
        sync_checkbox = Checkbutton(
            master=self, text=get_ui_string(ID.SETTINGS_ANKI_ENABLE_CONNECT),variable=sync_checkbox_value,
            onvalue=True, offvalue=False, command=None)
        sync_checkbox.grid(row=0, column=0, padx=5, pady=5, sticky=tkinter.W+tkinter.E)
