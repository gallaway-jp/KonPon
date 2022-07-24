from tkinter import Tk, messagebox
from tkinter.ttk import Frame
import tkinter

from context import save_settings
from context import Stats
from context import Theme, get_current_theme, set_dark_theme
from context import get_ui_string, StringID as ID

from gui_main_menu import MainMenu
from gui_action_frame import ActionFrame
from gui_list_treeview import TextList
from util.openfile import create_word_files

class KonPonGUI():
    def __init__(self):
        self.stats = Stats()

        #create window with particular properties
        self.window = _KonPonWindow()
        if get_current_theme() == Theme.DARK:
            set_dark_theme(parent=self.window)

        #populate window with the widgets
        self._add_widgets()

        self.window.protocol("WM_DELETE_WINDOW", self._on_close_window)

        #show window
        self.window.mainloop()
        
    def _add_widgets(self):
        frame = Frame(
            master=self.window, height=self.window.winfo_height(),
            width=self.window.winfo_width())
        frame.pack(fill=tkinter.BOTH, expand=True)
        frame.pack_propagate(False)

        #Add top area Menu e.g File, Tools, ...
        self.mainmenu = MainMenu(gui=self, main_window=self.window)

        #Add middle area list of all imported texts
        self.textlist = TextList(parent=frame, stats=self.stats)

        #Add bottom area frame that contains actions related to the texts
        self.action_frame = ActionFrame(
            parent=frame, textlist=self.textlist, known_words=self.stats.known_words,
                unknown_words=self.stats.unknown_words)
    
        self.window.bind('<Control-i>', self._on_ctrl_i_pressed)
        self.window.bind('<Control-w>', self._on_ctrl_w_pressed)
    
    def _on_ctrl_i_pressed(self, event) -> None:
        self.action_frame.handle_import_btn_clicked()

    def _on_ctrl_w_pressed(self, event) -> None:
        self.action_frame.handle_edit_btn_clicked()

    def _on_close_window(self):
        self.stats.save_known_words()
        self.stats.save_unknown_words()
        save_settings()

        self.window.destroy()

class _KonPonWindow(Tk):
    def __init__(self):
        super().__init__()

        
        #set window title to "KonPon"
        self.title(get_ui_string(ID.MAIN_TITLE))
        #set icon
        self.iconbitmap('./assets/KonPon.ico')

        self.set_window_size_position()

        #window is resizable 
        self.resizable(width=True, height=True) 
    
    def set_window_size_position(self):
        screen_percentage = .75

        self.minsize(width=300, height=400)

        #get screen dimensions
        screen_width = self.winfo_screenwidth()
        screen_height = self.winfo_screenheight()

        screen_ratio = 1.
        is_horizontal_screen = screen_width > screen_height
        if is_horizontal_screen:
            #get rotated screen ratio
            screen_ratio = screen_height / screen_width

        #calculate window dimensions
        window_width = int((screen_height if is_horizontal_screen else screen_width)
            * screen_ratio * screen_percentage)
        window_height = int((screen_width if is_horizontal_screen else screen_height)
            * screen_ratio * screen_percentage)

        # find the center point
        center_x = int(screen_width/2 - window_width / 2)
        center_y = int(screen_height/2 - window_height / 2)

        # set the position of the window to the center of the screen
        self.geometry(f'{window_width}x{window_height}+{center_x}+{center_y}')

if __name__ == '__main__':
    create_word_files()
    KonPonGUI()