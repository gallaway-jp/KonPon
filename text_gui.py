import tkinter
from tkinter import scrolledtext
from tkinter.ttk import Frame

from language import get_ui_string, StringID as ID
from openfile import open_file_as_string
from text_wordlists_gui import WordListsNotebook
from text_gui_action_frame import TextActionFrame
from text_gui_anki_page import AnkiPage

class TextDialog(tkinter.Toplevel):
    def __init__(
            self, file_path : str, words : dict, known_words : list, unknown_words : list) -> None:
        super().__init__()
        self.file_path = file_path
        self.words = words
        self.known_words = known_words
        self.unknown_words = unknown_words
        self.text = open_file_as_string(self.file_path)
        
        #set window properties
        self._init_window()

        #populate window with the widgets
        self._add_pages()
        
        #show window
        self.mainloop()
    def _init_window(self):
        #set window title to "Text"
        self.title(get_ui_string(ID.TEXT_TITLE))
        #set icon
        self.iconbitmap('./assets/KonPon.ico')

    def _add_pages(self):
        self.frame = tkinter.Frame(master=self)
        self.frame.pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=True)
        self.frame.grid_rowconfigure(0, weight=1)
        self.frame.grid_columnconfigure(0, weight=1)

        self.frames = {}
        for page in (StartPage, AnkiPage):
            page_name = page.__name__
            frame = page(parent=self.frame, controller=self)
            self.frames[page_name] = frame

            # put all of the pages in the same location;
            # the one on the top of the stacking order
            # will be the one that is visible.
            frame.grid(row=0, column=0, sticky=tkinter.NSEW)
        self.show_frame("StartPage")

    def show_frame(self, page_name, opt=None):
        '''Show a frame for the given page name'''
        frame = self.frames[page_name]
        frame.initialize()

        if page_name == "AnkiPage":
            frame.insert_words(is_known_words=opt)

        frame.tkraise()

class StartPage(Frame):
    def __init__(self, parent : Frame, controller: TextDialog) -> None:
        super().__init__(master=parent)
        self.file_path = controller.file_path
        self.words = controller.words
        self.known_words = controller.known_words
        self.unknown_words = controller.unknown_words
        self.text = controller.text

        self.configure_col_row()

        self.nb_word_lists = WordListsNotebook(master=self, words=self.words,
            known_words=self.known_words, unknown_words=self.unknown_words,text_dialog=self)
        self.text_scrolledtext = scrolledtext.ScrolledText(master=self, wrap=tkinter.WORD, width=100, height=20)
        self._add_text()

        self.nb_word_lists.grid(row=0, column=0, padx=5, pady=5, rowspan=6, columnspan=3, sticky=tkinter.NSEW)
        self.text_scrolledtext.grid(row=0, column=4, padx=5, pady=5, rowspan=6,columnspan=3, sticky=tkinter.NSEW)

        self.action_frame = TextActionFrame(parent=self, controller=controller, textlist=self.words)

        self.text_scrolledtext.bind("<Key>", self.on_key_pressed)
    
    def initialize(self) -> None:
        return
    
    def configure_col_row(self) -> None:
        self.columnconfigure(index=tuple(range(7)), weight=1)
        self.rowconfigure(index=tuple(range(6)), weight=1)
        self.rowconfigure(index=6, weight=0)
    
    def set_cursor_pos(self, index : int):
        self.text_scrolledtext.mark_set(tkinter.INSERT, "1.0+%d chars" % index)
    def go_to_pos(self, index : int) -> None:
        self.text_scrolledtext.see("1.0+%d chars" % index)
    def select_text(self, index : int, length : int) -> None:
        self.text_scrolledtext.tag_remove(tkinter.SEL, "1.0", tkinter.END)

        index_end = index+length
        index = int(index)
        self.text_scrolledtext.tag_add(tkinter.SEL, "1.0+%d chars" % index, "1.0+%d chars" % index_end)

    def on_key_pressed(self, event : tkinter.Event) -> None:
        #event.state has the following masks
        # 0x0001 Shift, 0x0002 Caps Lock, 0x0004 Control, 0x0008 Left Alt
        # 0x0010 Num Lock, 0x0080 Right Alt, 0x0100 Mouse 1,
        # 0x0200 Mouse 2, 0x0400 Mouse 3
        match event.keycode:
            case 9: #'Tab' key
                widget : tkinter.Widget = event.widget
                if event.state & 0x0001: #Shift → previous widget
                    widget.tk_focusPrev().focus_set() #set focus to previous widget
                else:
                    widget.tk_focusNext().focus_set() #set focus to next widget
            case 37 | 38 | 39 | 40: #left, up, right, down keys
                return #let key press event happen: move caret left, up, right, down
            case 65: #'a' key
                if event.state & 0x0004: #'ctrl' key
                    return #let key press event happen: select all text
            case 67: #'c' key
                if event.state & 0x0004: #'ctrl' key
                    return #let key press event happen: copy text
            case _:
                print(f'{event.state}, {event.keycode}', {event.keysym})
        return "break" #ignore key press event

    def _add_text(self) -> None:
        self.text_scrolledtext.insert(tkinter.END, self.text)

if __name__ == '__main__':
    pass