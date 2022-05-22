import tkinter
from tkinter import scrolledtext
from tkinter.ttk import Frame, Label, LabelFrame, Button

from language import get_ui_string, StringID as ID

class AddWordsDialog():
    def __init__(self, known_listbox : tkinter.Listbox, unknown_listbox : tkinter.Listbox,
            set_open_flag: object) -> None:
        self.known_listbox = known_listbox
        self.unknown_listbox = unknown_listbox
        self.set_open_flag = set_open_flag

        self.bool_HasChanges = False
        
        #create window with particular properties
        self.window = self._init_window()

        #populate window with the widgets
        self._add_widgets()
        
        #show window
        self.window.mainloop()
    def _init_window(self):
        window = tkinter.Toplevel()

        #set window title to "Add Words"
        window.title(get_ui_string(ID.ADD_WORDS_TITLE))
        #set icon
        window.iconbitmap('./assets/KonPon.ico')

        #window.geometry('400x300')

        #window is NOT resizable 
        window.resizable(False, False) 

        return window
    def _add_widgets(self):
        self.frame = Frame(master = self.window)

        self.frame.rowconfigure(index=tuple(range(6)), weight=1)
        self.frame.rowconfigure(index=6, weight=0)
        self.frame.columnconfigure(index=tuple(range(7)), weight=1)

        unknown_label = Label(master = self.frame, text=get_ui_string(ID.ADD_WORDS_UNKNOWN_WORDS))
        known_label = Label(master = self.frame, text=get_ui_string(ID.ADD_WORDS_KNOWN_WORDS))
        unknown_frame = LabelFrame(master = self.frame, labelwidget=unknown_label)
        known_frame = LabelFrame(master = self.frame, labelwidget=known_label)

        self.unknown_scrolledtext = scrolledtext.ScrolledText(master=unknown_frame, wrap=tkinter.WORD, width=50, height=20)
        self.known_scrolledtext = scrolledtext.ScrolledText(master=known_frame, wrap=tkinter.WORD, width=50, height=20)
        self.unknown_scrolledtext.pack(fill=tkinter.BOTH, expand=True)
        self.known_scrolledtext.pack(fill=tkinter.BOTH, expand=True)

        btn_save = Button(master=self.frame,
            text=get_ui_string(ID.ADD_WORDS_CONFIRM), command=self.on_save_btn_clicked)

        unknown_frame.grid(row=0, column=0, padx=5, pady=5, rowspan=6, columnspan=3, sticky=tkinter.NSEW)
        known_frame.grid(row=0, column=4, padx=5, pady=5, rowspan=6,columnspan=3, sticky=tkinter.NSEW)
        btn_save.grid(row=6, column=6, padx=5, pady=5, sticky=tkinter.EW)

        self.frame.pack()

        self.window.protocol("WM_DELETE_WINDOW", self._destroy_window)
        self.window.bind('<Escape>', self._on_close_window)
        self.window.bind('<Control-Return>', self.on_ctrl_enter_pressed)

    def _on_close_window(self, event) -> None:
        self._destroy_window()

    def _destroy_window(self) -> None:
        self.set_open_flag(False)
        self.window.destroy()
    
    def on_save_btn_clicked(self):
        if True: #self.bool_HasChanges:
            self.update_wordlists()
        self._destroy_window()
    
    def on_ctrl_enter_pressed(self, event):
        self.on_save_btn_clicked()

    def update_wordlists(self):
        self.get_words_from_scrolledtext(words_scrolledtext=self.unknown_scrolledtext, listbox=self.unknown_listbox)
        self.get_words_from_scrolledtext(words_scrolledtext=self.known_scrolledtext, listbox=self.known_listbox)

    def get_words_from_scrolledtext(self, words_scrolledtext: scrolledtext.ScrolledText, listbox : tkinter.Listbox):
        words = words_scrolledtext.get('0.0', tkinter.END).splitlines()
        listbox_words = [str(item) for item in listbox.get(first=0, last=tkinter.END)]
        for word in words:
            if word != '' and word not in listbox_words:
                listbox.insert(tkinter.END, word)

if __name__ == '__main__':
    AddWordsDialog(tkinter.Listbox(), tkinter.Listbox(), lambda _ : True)