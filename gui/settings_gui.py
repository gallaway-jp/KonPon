import tkinter
from tkinter.ttk import Notebook

from settings_gui_tab import GUITab
from settings_file_tab import FileTab
from settings_anki_tab import AnkiTab
from language import get_ui_string, StringID as ID

class SettingsWindow(tkinter.Toplevel):
    """
    A class to represent the settings window of the app.

        Attributes:
            notebook (Notebook): Notebook containing File, GUI tabs in SettingsWindow window

        Methods:
            _init_window(): Initializes properties of window
            _add_notebook(): Creates and returns a notebook containing File, GUI tabs
    """
    def __init__(self) -> None:
        """
        Constructs all the necessary attributes for the SettingsWindow object.

            Parameters:
                N/A
        """
        super().__init__()

        #Initialize properties of window
        self._init_window()

        #populate window with a notebook
        self.notebook = self._add_notebook()
    
        #show window
        self.mainloop()

    def _init_window(self) -> None:
        """
        Initializes properties of window.

            Parameters:
                N/A

            Returns:
                None
        """
        #set window title to "Settings"
        self.title(get_ui_string(ID.SETTINGS_TITLE))
        #set icon
        self.iconbitmap('./assets/KonPon.ico')
        #set window size
        self.geometry('400x300')
        #window is NOT resizable 
        self.resizable(False, False) 

    def _add_notebook(self) -> Notebook:
        """
        Creates and returns a notebook containing File, GUI tabs.

            Parameters:
                N/A

            Returns:
                notebook (Notebook): Notebook containing File, GUI tabs
        """        
        notebook = Notebook(master=self, width=400, height=280)
        notebook.pack(pady=0, expand=True)

        file_frame = FileTab(notebook=notebook)
        GUI_frame = GUITab(notebook=notebook)
        GUI_frame = AnkiTab(notebook=notebook)

        return notebook

if __name__ == '__main__':
    SettingsWindow()