from tkinter import END, NO, CENTER, VERTICAL
from tkinter.ttk import Treeview, Scrollbar

#from text_gui import TextDialog


class TextWordList(Treeview):
    def __init__(self, master=None, words: dict = {}, text_dialog = None) -> None:
        super().__init__(master, columns=('count'), show='tree')
        self.words : dict = words
        self.text_dialog = text_dialog

        #self.heading('count', text='Number of Occurrences')

        # adding data
        self.insert_data()

        scrollbar = Scrollbar(master=master, orient = VERTICAL, command = self.yview)
        scrollbar.pack(side ='right', fill ='y')
        self.configure(yscrollcommand = scrollbar.set)

        self.bind("<Double-1>", self.on_index_selected)
        #self.bind("<Button-1>", self.on_index_selected)

    def insert_data(self):
        for (word, indices) in self.words.items():
            self.insert('', END, text=word, iid=word, open=False, values=[len(indices.keys())])
            is_first = True
            for index in indices.keys():
                if is_first:
                    is_first = False
                    continue #details info, not index!!!
                self.insert(word, END, text=index, iid=index, open=False)

    def on_index_selected(self, event) -> None:
        item = self.identify_row(event.y)
        if item == '' or not item.isdigit():
            return
        iid = int(item)
        parent_iid = self.parent(iid)
        print(f'{parent_iid}:{iid}')
        self.text_dialog.set_cursor_pos(index=iid)
        self.text_dialog.go_to_pos(index=iid)
        self.text_dialog.select_text(index=iid, length=self.words[parent_iid][iid])
        