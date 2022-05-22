import tkinter
from tkinter.ttk import Frame, Label, Combobox, Notebook

from language import Language, get_ui_string, StringID as ID
from theme import Theme

# def handle_theme_cmb_clicked(event, cmb, theme) -> None:
#     match cmb.get():
#             case 'DEFAULT' | 'LIGHT':
#                 theme.change_theme(Theme.LIGHT)
#             case 'DARK':
#                 theme.change_theme(Theme.DARK)

# def handle_language_cmb_clicked(event, cmb, language) -> None:
#     match cmb.get():
#             case 'DEFAULT':
#                 language.change_language(Language.DEFAULT)
#             case 'EN':
#                 language.change_theme(Language.EN)
#             case 'JA':
#                 language.change_theme(Language.JA)

class GUITab(Frame):
    def __init__(self, notebook : Notebook) -> None:
        super().__init__(master=notebook)

        self._configure_rows_cols()

        self._add_widgets_to_tab()

        self.pack(fill=tkinter.BOTH, expand=True)

        notebook.add(child=self, text=get_ui_string(ID.SETTINGS_GUI))

    def _configure_rows_cols(self) -> None:
        self.columnconfigure(index=tuple(range(2)), weight=1)
        self.rowconfigure(index=tuple(range(2)), weight=1)

    def _add_widgets_to_tab(self) -> None:
        self._add_theme_settings_to_tab()
        self._add_language_settings_to_tab()

    def _add_theme_settings_to_tab(self):
        theme_label = Label(master=self, text=get_ui_string(ID.SETTINGS_GUI_THEME))
        theme_label.grid(row=0, column=0, padx=5, pady=5, sticky=tkinter.W+tkinter.E)

        theme_list = [str(Theme.DEFAULT), str(Theme.LIGHT), str(Theme.DARK)]
        theme_combobox = Combobox(master=self, values=theme_list)
        theme_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -foreground #FEFEFE' % theme_combobox)
        theme_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -background #3A3A3A' % theme_combobox)
        theme_combobox.set(theme_list[0])
        theme_combobox['state'] = 'readonly'
        
        theme_combobox.grid(row=0, column=1, padx=5, pady=5, sticky=tkinter.W+tkinter.E)
        #theme_combobox.bind('<<ComboboxSelected>>', lambda event, cmb=theme_combobox, thm=theme: handle_theme_cmb_clicked(event, cmb, thm))

    def _add_language_settings_to_tab(self):
        theme_label = Label(master=self, text=get_ui_string(ID.SETTINGS_GUI_LANGUAGE))
        theme_label.grid(row=1, column=0, padx=5, pady=5, sticky=tkinter.W+tkinter.E)

        theme_list = [str(Language.DEFAULT), str(Language.EN), str(Language.JA)]
        language_combobox = Combobox(master=self, values=theme_list)
        language_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -foreground #FEFEFE' % language_combobox)
        language_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -background #3A3A3A' % language_combobox)

        language_combobox.set(theme_list[0])
        language_combobox['state'] = 'readonly'
        
        language_combobox.grid(row=1, column=1, padx=5, pady=5, sticky=tkinter.W+tkinter.E)
        #language_combobox.bind('<<ComboboxSelected>>', lambda event, cmb=language_combobox, lang=language: handle_language_cmb_clicked(event, cmb, lang))