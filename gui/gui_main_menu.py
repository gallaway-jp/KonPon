import tkinter
from tkinter import messagebox

from language import get_ui_string, StringID as ID
from settings_gui import SettingsWindow

APP_VERSION = "1.00"

class MainMenu(tkinter.Menu):
    """
    A class to represent the main menu of the app.

        Attributes:
            main_window (tkinter.Tk): Main window of app and parent widget of MainMenu

        Methods:
            _add_file_menu(): Creates and returns a file menu
            _add_tools_menu(): Creates and returns a tools menu
            _add_help_menu(): Creates and returns a help menu
    """
    def __init__(self, gui, main_window : tkinter.Tk) -> None:
        """
        Constructs all the necessary attributes for the MainMenu object.

            Parameters:
                main_window (tkinter.Tk): Main window of app and parent widget of MainMenu
        """
        super().__init__(master=main_window)
        self.gui = gui
        self.main_window = main_window
        self.main_window.config(menu=self)

        self.file_menu = self._add_file_menu()
        self.tools_menu = self._add_tools_menu()
        self.help_menu = self._add_help_menu()
    
    def _add_file_menu(self) -> tkinter.Menu:
        """
        Creates and returns a file menu.

            Parameters:
                N/A

            Returns:
                file_menu (tkinter.Menu): Menu for file-related commands
        """
        file_menu = tkinter.Menu(master=self, tearoff=False)
        self.add_cascade(label=get_ui_string(ID.MAIN_MENU_FILE), menu=file_menu)
        file_menu.add_command(
            label=get_ui_string(ID.MAIN_MENU_FILE_IMPORT_TEXT), command=self._on_import_text_cmd_clicked)
        file_menu.add_command(
            label=get_ui_string(ID.MAIN_MENU_FILE_VIEW_WORD_LISTS), command=self._on_edit_wordlists_cmd_clicked)
        file_menu.add_command(
            label=get_ui_string(ID.MAIN_MENU_FILE_EXIT), command=self.main_window.quit)
        return file_menu
    
    def _on_import_text_cmd_clicked(self) -> None:
        self.gui._on_ctrl_i_pressed(None)

    def _on_edit_wordlists_cmd_clicked(self) -> None:
        self.gui._on_ctrl_w_pressed(None)

    def _add_tools_menu(self) -> tkinter.Menu:
        """
        Creates and returns a tools menu.

            Parameters:
                N/A

            Returns:
                tools_menu (tkinter.Menu): Menu for tools-related commands
        """
        tools_menu = tkinter.Menu(master=self, tearoff=False)
        self.add_cascade(label=get_ui_string(ID.MAIN_MENU_TOOLS), menu=tools_menu)
        tools_menu.add_command(label=get_ui_string(ID.MAIN_MENU_TOOLS_SETTINGS), command=SettingsWindow)
        return tools_menu

    def _add_help_menu(self) -> tkinter.Menu:
        """
        Creates and returns a help menu.

            Parameters:
                N/A

            Returns:
                help_menu (tkinter.Menu): Menu for help-related commands
        """
        help_menu = tkinter.Menu(master=self, tearoff=False)
        self.add_cascade(label=get_ui_string(ID.MAIN_MENU_HELP), menu=help_menu)
        help_menu.add_command(label=get_ui_string(ID.MAIN_MENU_HELP_ABOUT), command=self._show_about_dialog)
        return help_menu

    def _show_about_dialog(self) -> None:
        """
        Displays program info in a showinfo dialog.

            Parameters:
                N/A

            Returns:
                None
        """
        info_text = get_ui_string(ID.MAIN_MENU_HELP_ABOUT_INFO)
        messagebox.showinfo(title=get_ui_string(ID.MAIN_MENU_HELP_ABOUT_TITLE),
            message=f"{info_text}{APP_VERSION}")