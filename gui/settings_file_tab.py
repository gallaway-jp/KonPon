import tkinter
from tkinter.ttk import Frame, Button, Entry, Notebook
from language import get_ui_string, StringID as ID

import util.openfile as openfile
from theme import Space

class FileTab(Frame):
    """
    A class to represent the settings window of the app.

        Attributes:
            notebook (Notebook): Notebook containing File, GUI tabs in SettingsWindow window

        Methods:
            _configure_rows_cols(): Configures the rows and columns of the frame's grid layout
            _add_widgets_to_tab(): Adds widgets to the tab frame
            _add_workspace_settings_to_tab(): Adds widgets to the tab frame to change workspace settings
            _handle_set_directory_btn_clicked(): Sets directory when button is clicked
    """
    def __init__(self, notebook : Notebook) -> None:
        super().__init__(master=notebook)
        self.notebook = notebook

        self._configure_rows_cols()

        self._add_widgets_to_tab()

        self.pack(fill=tkinter.BOTH, expand=True)

        self.notebook.add(child=self, text=get_ui_string(ID.SETTINGS_FILE))

    def _configure_rows_cols(self) -> None:
        self.columnconfigure(index=0, weight=1)
        self.rowconfigure(index=0, weight=1)

    def _add_widgets_to_tab(self) -> None:
        self._add_workspace_settings_to_tab()
    
    def _add_workspace_settings_to_tab(self):
        entry_change_workspace = Entry(master=self, text="")
        entry_change_workspace.insert(0, "")
        entry_change_workspace.configure(state="readonly")

        btn_change_workspace = Button(master=self, text=get_ui_string(ID.SETTINGS_FILE_CHANGE_WORKSPACE))
        
        entry_change_workspace.grid(row=0, column=0, padx=5, pady=5, columnspan = 2, sticky=tkinter.W+tkinter.E)
        btn_change_workspace.grid(row=0, column=2, padx=5, pady=5, sticky=tkinter.E)

        btn_change_workspace.bind("<Button-1>", self._handle_set_directory_btn_clicked)

    def _handle_set_directory_btn_clicked(self, event : tkinter.Event):
        directory = openfile.get_directory()
        print(f"Directory:{directory}")
