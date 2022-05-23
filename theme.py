from enum import Enum
import tkinter

from ini import Theme, get_settings

#dark theme surface color is #121212
DARK_SURFACE_COLOR = "#121212"

class Space(Enum):
    BOTH = 1 #System
    FG = 2
    BG = 3

    def __str__(self) -> str:
        return str(self.name)

class ThemeClass:
    def __init__(self) -> None:
        self.widgets = list()
        
    def change_theme(self, theme) -> None:
        match theme:
            case Theme.DEFAULT | Theme.LIGHT:
                [widget.configure(fg ="black",bg="white") if space == Space.BOTH\
                    else widget.configure(bg="white") if space == Space.BG\
                        else widget.configure(fg="black")\
                            for widget, space in self.widgets]
            case Theme.DARK:
                #[configure(fg ="white",bg="black") for configure in self.widgets_configs]
                [widget.configure(fg ="white",bg=DARK_SURFACE_COLOR) if space == Space.BOTH\
                    else widget.configure(bg=DARK_SURFACE_COLOR) if space == Space.BG\
                        else widget.configure(fg ="white")\
                            for widget, space in self.widgets]
    
    def add_widget(self, widget, space=Space.BOTH) -> None:
        self.widgets.append((widget, space))

DEFAULT_THEME = Theme.DEFAULT
CURRENT_THEME = get_settings("GUI", "Theme")

def get_default_theme() -> Theme:
    global DEFAULT_THEME
    if DEFAULT_THEME == Theme.DEFAULT:
        DEFAULT_THEME = update_default_theme()
    return DEFAULT_THEME

def get_current_theme() -> Theme:
    global CURRENT_THEME
    if CURRENT_THEME == Theme.DEFAULT:
        CURRENT_THEME = get_default_theme()
    return CURRENT_THEME

def set_current_theme(theme : Theme) -> None:
    global CURRENT_THEME
    CURRENT_THEME = theme

def update_default_theme() -> Theme:
    import darkdetect
    theme = darkdetect.theme()
    match theme:
        case "Dark": 
            return Theme.DARK
        case "Light", None, _ :
            return Theme.LIGHT

from tkinter import Misc
from tkinter.ttk import Style
def set_dark_theme(parent : Misc) -> None:
    style = Style()
    style.theme_use('default')
    style.configure("TButton",
        foreground="#EDEDED", background="#606060", borderwidth=1, focusthickness=3, justify=tkinter.CENTER)
    style.map('TButton', background=[('active',"#656565")])

    #style.configure("TCheckbutton.label", foreground="#FFFFFF", background="#2F2F31")
    style.configure("TCheckbutton", foreground="#FFFFFF", background="#2F2F31")
    style.map('TCheckbutton', background=[('active',"#2F2F31")])

    style.configure("TEntry", foreground="#FEFEFE", fieldbackground="#3A3A3A")
    style.map('TEntry', foreground=[('readonly',"#FEFEFE")], fieldbackground=[('readonly',"#3A3A3A")])

    style.configure('TLabel', foreground="#FFFFFF", background="#2F2F31")
    style.configure('TLabelframe', background="#2F2F31")

    style.configure('TCombobox', foreground="#FEFEFE", fieldbackground="#3A3A3A", selectbackground="red", background="#606060")
    style.map('TCombobox', background=[('active',"#656565")])
    #*TCombobox*Listbox.foreground
    #root.option_add('*TCombobox*Listbox.foreground' % frame, 'red')
    #missed_list_combobox.tk.eval('[ttk::combobox::PopdownWindow %s].f.l configure -foreground red' % missed_list_combobox)
    style.map('TCombobox', foreground=[('readonly',"#FEFEFE")], fieldbackground=[('readonly',"#3A3A3A")])

    style.configure('TFrame', background="#2F2F31")
    style.configure('Treeview', foreground="#FEFEFE", background="#3A3A3A")
    style.layout("Treeview", [('Treeview.treearea', {'sticky': 'nswe'})])
    style.configure('Treeview.Heading', foreground="#FEFEFE", background="#3A3A3A")
    style.map('Treeview.Heading', background=[('active',"#3F3F3F")])
    style.configure("TSeparator", background="black")
    #style.element_create("Vertical.TScrollbar", "from", "default")
    style.configure("Vertical.TScrollbar", background="#656565", troughcolor="#2F2F31")
    style.map('Vertical.TScrollbar', background=[('disabled',"#2F2F31")])

    style.configure('TNotebook.Tab', foreground="#FFFFFF", background="#2F2F31")
    style.map('TNotebook.Tab', background=[('active',"#343436")])
    style.configure('TNotebook', background="#2F2F31")  