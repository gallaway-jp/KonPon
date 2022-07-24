import tkinter
from tkinter.ttk import Frame, Label, LabelFrame, Button, Scrollbar

from language import get_ui_string, StringID as ID
from wordlist_add_gui import AddWordsDialog

class Wordlist():
    def __init__(self, known_words : list, unknown_words : list) -> None:
        self.known_words = known_words
        self.unknown_words = unknown_words

        self.bool_HasChanges = False
        self.bool_add_words_dialog_is_open = False
        
        #create window with particular properties
        self.window = self._init_window()

        #populate window with the widgets
        self._add_widgets()

        self.add_words_to_listboxes()
        
        #show window
        self.window.mainloop()
    def _init_window(self):
        window = tkinter.Toplevel()

        #set window title to "Word Lists"
        window.title(get_ui_string(ID.WORD_LISTS_TITLE))
        #set icon
        window.iconbitmap('./assets/KonPon.ico')

        #window.geometry('400x300')
        window.minsize(width=300, height=200)

        #window is resizable 
        window.resizable(True, True) 

        return window
    def _add_widgets(self):
        self.frame = Frame(master = self.window)

        self.frame.rowconfigure(index=tuple(range(6)), weight=1)
        self.frame.rowconfigure(index=6, weight=0)
        self.frame.columnconfigure(index=tuple(range(3)), weight=1)
        self.frame.columnconfigure(index=3, weight=0)
        self.frame.columnconfigure(index=tuple(range(4,7)), weight=1)

        unknown_label = Label(master = self.frame, text=get_ui_string(ID.WORD_LISTS_UNKNOWN_WORDS))
        known_label = Label(master = self.frame, text=get_ui_string(ID.WORD_LISTS_KNOWN_WORDS))
        unknown_frame = LabelFrame(master = self.frame, labelwidget=unknown_label)
        known_frame = LabelFrame(master = self.frame, labelwidget=known_label)

        #Add listboxes for each word list
        self.unknown_listbox = tkinter.Listbox(master=unknown_frame, width=50,height=10, selectmode=tkinter.EXTENDED)
        self.known_listbox = tkinter.Listbox(master=known_frame, width=50,height=10, selectmode=tkinter.EXTENDED)
        self.unknown_listbox.pack(side=tkinter.RIGHT, fill=tkinter.BOTH, expand=True)
        self.known_listbox.pack(side=tkinter.LEFT, fill=tkinter.BOTH, expand=True)

        #Add scrollbars to each listbox
        unknown_scrollbar = Scrollbar(master=unknown_frame, orient=tkinter.VERTICAL)
        unknown_scrollbar.config(command=self.unknown_listbox.yview)
        self.unknown_listbox.config(yscrollcommand=unknown_scrollbar.set)
        known_scrollbar = Scrollbar(master=known_frame, orient=tkinter.VERTICAL)
        known_scrollbar.config(command=self.known_listbox.yview)
        self.known_listbox.config(yscrollcommand=known_scrollbar.set)
        unknown_scrollbar.pack(side=tkinter.LEFT, fill=tkinter.BOTH)
        known_scrollbar.pack(side=tkinter.RIGHT, fill=tkinter.BOTH)

        # Add buttons to move words, add words, delete words, and save lists
        btn_move_all_to_unknown = Button(master=self.frame, text="⇇", command=self.make_all_words_unknown)
        btn_move_to_unknown = Button(master=self.frame, text="←", command=self.make_words_unknown)
        btn_move_to_known = Button(master=self.frame, text="→", command=self.make_words_known)
        btn_move_all_to_known = Button(master=self.frame, text="⇉", command=self.make_all_words_known)
        btn_add = Button(master=self.frame,
            text=get_ui_string(ID.WORD_LISTS_ADD), command=self.on_add_btn_clicked)
        btn_delete = Button(master=self.frame,
            text=get_ui_string(ID.WORD_LISTS_DELETE), command=self.on_delete_btn_clicked)
        btn_save = Button(master=self.frame,
            text=get_ui_string(ID.WORD_LISTS_CONFIRM), command=self.on_save_btn_clicked)

        #Place widgets in window
        unknown_frame.grid(row=0, column=0, padx=5, pady=5, rowspan=6, columnspan=3, sticky=tkinter.NSEW)
        btn_move_all_to_unknown.grid(row=1, column=3, padx=5, pady=5, sticky=tkinter.EW)
        btn_move_to_unknown.grid(row=2, column=3, padx=5, pady=5, sticky=tkinter.EW)
        btn_move_to_known.grid(row=3, column=3, padx=5, pady=5, sticky=tkinter.EW)
        btn_move_all_to_known.grid(row=4, column=3, padx=5, pady=5, sticky=tkinter.EW)
        known_frame.grid(row=0, column=4, padx=5, pady=5, rowspan=6,columnspan=3, sticky=tkinter.NSEW)
        btn_add.grid(row=6, column=4, padx=5, pady=5, sticky=tkinter.EW)
        btn_delete.grid(row=6, column=5, padx=5, pady=5, sticky=tkinter.EW)
        btn_save.grid(row=6, column=6, padx=5, pady=5, sticky=tkinter.EW)

        self.frame.pack(fill=tkinter.BOTH, expand=True)

        self.window.bind("<Return>", self.on_enter_key_pressed)
        self.unknown_listbox.bind('<Right>', self.on_right_key_pressed)
        self.known_listbox.bind('<Left>', self.on_left_key_pressed)

        self.window.bind('<Escape>', self._on_close_window)
        self.window.bind('<Control-i>', self.on_ctrl_i_pressed)
        self.window.bind("<Delete>", self.on_delete_key_pressed)
        #self.window.protocol("WM_DELETE_WINDOW", self._on_close_window)
    
    def _on_close_window(self, event : tkinter.Event) -> None:
        self.window.destroy()

    def on_ctrl_i_pressed(self, event : tkinter.Event) -> None:
        self.on_add_btn_clicked()

    def on_delete_key_pressed(self, event : tkinter.Event) -> None:
        self.on_delete_btn_clicked()

    def add_words_to_listboxes(self):
        for index, word in enumerate(self.unknown_words):
            self.unknown_listbox.insert(index, word)
        for index, word in enumerate(self.known_words):
            self.known_listbox.insert(index, word)
    
    def on_save_btn_clicked(self):
        if self.bool_HasChanges:
            self.update_wordlists()
        self.window.destroy()
    
    def on_enter_key_pressed(self, event):
        self.on_save_btn_clicked()

    def update_wordlists(self):
        if self.select_all_words(self.unknown_listbox):
            self.unknown_words.clear()
            self.get_words_from_listbox(self.unknown_listbox, self.unknown_words)
        if self.select_all_words(self.known_listbox):
            self.known_words.clear()
            self.get_words_from_listbox(self.known_listbox, self.known_words)

    def get_words_from_listbox(self, listbox : tkinter.Listbox, wordlist : list):
        index_list = listbox.curselection()
        if index_list:
            for index in index_list:
                val = listbox.get(first=index)
                wordlist.append(val)

    def on_right_key_pressed(self, event):
        self.make_words_known()

    def on_left_key_pressed(self, event):
        self.make_words_unknown()

    def set_add_words_dialog_is_open_flag(self, bool_flag : bool):
        self.bool_add_words_dialog_is_open = bool_flag

    def on_add_btn_clicked(self):
        if not self.bool_add_words_dialog_is_open:
            self.set_add_words_dialog_is_open_flag(bool_flag=True)
            AddWordsDialog(known_listbox=self.known_listbox, unknown_listbox=self.unknown_listbox,
                set_open_flag=lambda bool_flag=False : self.set_add_words_dialog_is_open_flag(bool_flag))

    def on_delete_btn_clicked(self):
        self.delete_words(self.unknown_listbox)
        self.delete_words(self.known_listbox)

    def make_words_unknown(self):
        self._move_words(from_list=self.known_listbox, to_list=self.unknown_listbox)
    def make_words_known(self):
        self._move_words(from_list=self.unknown_listbox, to_list=self.known_listbox)
    def make_all_words_unknown(self):
        self._move_all_words(from_list=self.known_listbox, to_list=self.unknown_listbox)
    def make_all_words_known(self):
        self._move_all_words(from_list=self.unknown_listbox, to_list=self.known_listbox)

    def _move_words(self, from_list : tkinter.Listbox, to_list : tkinter.Listbox):
        index_list = from_list.curselection()
        if index_list:
            self.bool_HasChanges = True
            for index in index_list:
                val = from_list.get(first=index)
                to_list.insert(tkinter.END, val)
            for index in reversed(index_list):
                from_list.delete(first=index)
    def _move_all_words(self, from_list : tkinter.Listbox, to_list : tkinter.Listbox):
        if not self.select_all_words(from_list):
            return
        self._move_words(from_list=from_list, to_list=to_list)

    # def append_words(self, words : list, listbox : tkinter.Listbox):
    #     for word in words:
    #         listbox.insert(tkinter.END, word)

    def delete_words(self, listbox : tkinter.Listbox):
        index_list = listbox.curselection()
        if index_list:
            self.bool_HasChanges = True
            for index in reversed(index_list):
                listbox.delete(first=index)

    def select_all_words(self, listbox : tkinter.Listbox) -> bool:
        list_size = listbox.size()
        if list_size <= 0:
            return False
        listbox.selection_set(first=0,last=list_size-1)
        return True

if __name__ == '__main__':
    pass