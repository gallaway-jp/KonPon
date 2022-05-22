from textwrap import fill
import tkinter
from tkinter.ttk import Treeview, Style, Scrollbar
from os import scandir, path as os_path, remove as remove_path
from pathlib import Path
from language import get_ui_string, StringID as ID

from stats import Stats
from text_gui import TextDialog
from openfile import delete_folder_with_items

class TextList():
    def __init__(self, parent : tkinter.Misc, stats : Stats) -> None:
        self.parent = parent
        self.current_iid = 0
        self.text_dict = dict()
        self.stats = stats

        frm_list = tkinter.Frame(master=parent)#, height=parent.winfo_height(), width=parent.winfo_width()) #, bg='red'
        frm_list.pack(fill='both',expand=True)
        #frm_list.pack_propagate(False)
        
        self.tv = Treeview(master=frm_list)
        self.tv['columns']=('Name', 'Date', '% Words Known')
        self.tv.column('#0', width=0, stretch=tkinter.NO)
        self.tv.column('Name', anchor=tkinter.CENTER, width=80)
        self.tv.column('Date', anchor=tkinter.CENTER, width=80)
        self.tv.column('% Words Known', anchor=tkinter.CENTER, width=80)

        self.tv.heading('#0', text="", anchor=tkinter.CENTER)
        self.tv.heading('Name', text=get_ui_string(ID.TEXT_LIST_NAME), anchor=tkinter.CENTER)
        self.tv.heading('Date', text=get_ui_string(ID.TEXT_LIST_DATE), anchor=tkinter.CENTER)
        self.tv.heading('% Words Known', text=get_ui_string(ID.TEXT_LIST_PERCENTAGE), anchor=tkinter.CENTER)

        self.tv.pack(fill=tkinter.BOTH, side=tkinter.LEFT, expand=True)

        scrollbar = Scrollbar(master=frm_list, orient = tkinter.VERTICAL, command = self.tv.yview)
        scrollbar.pack(side=tkinter.RIGHT, fill=tkinter.Y)
        self.tv.configure(yscrollcommand = scrollbar.set)

        self.rightclick_menu = tkinter.Menu(frm_list, tearoff=False)
        self.rightclick_menu.add_command(label=get_ui_string(ID.TEXT_LIST_RIGHT_CLICK_COPY_PATH), command=self.copy_file_path)
        self.rightclick_menu.add_command(label=get_ui_string(ID.TEXT_LIST_RIGHT_CLICK_DELETE), command=self.delete)

        self.tv.bind("<Double-1>", self.OnDoubleClick)
        self.tv.bind("<Return>", self.OnEnterKeyPressed)
        self.tv.bind("<Button-3>", self.OnRightClick)
        self.tv.bind("<Delete>", self.OnDeleteKeyPressed)

        self._load_preexisting_items()
    
    def insert(self, name, time, percentage, iid):
        self.tv.insert(parent='', index=0, iid=iid, text='', values=(name, time,str(percentage)))
    
    def delete(self):
        selected_items = self.tv.selection() # get selected items
        for selected_item in selected_items:
            path, _, _ = self.text_dict[int(selected_item)]
            if delete_folder_with_items(path=path):
                self.tv.delete(selected_item)

    def OnDeleteKeyPressed(self, event):
        self.delete()

    def OnEnterKeyPressed(self, event):
        iid = 0
        try:
            iid = int(self.tv.selection()[0])
        except IndexError as error:
            pass
            #print(error)
        else:
            print(iid)
            print(self.text_dict[iid])

            (path, name, date) = self.text_dict[iid]
            txt_path = str(Path(path, name).with_suffix(".txt"))
            csv_path = str(Path(txt_path).with_suffix(".csv"))
            TextDialog(txt_path, self.stats.get_text_data(csv_path),
                self.stats.known_words, self.stats.unknown_words)
    def OnDoubleClick(self, event):
        item = self.tv.identify_row(event.y)
        if item == '':
            return
        iid = int(item)
        print(iid)
        print(self.text_dict[iid])

        (path, name, date) = self.text_dict[iid]
        txt_path = str(Path(path, name).with_suffix(".txt"))
        csv_path = str(Path(txt_path).with_suffix(".csv"))
        TextDialog(
            txt_path, self.stats.get_text_data(csv_path), self.stats.known_words, self.stats.unknown_words)


    def OnRightClick(self, event):
        #item = self.tv.identify('item', event.x, event.y)
        item = self.tv.identify_row(event.y)
        if item == '':
            return
        print(int(item))
        self.tv.focus(item)
        self.tv.selection_set(item)
        self.rightclick_menu.tk_popup(event.x_root,event.y_root)

    def load_new_item(self, name : str, date, path : str, word_dict : dict):
        iid = self.getNextIid()
        date = date[:-6]
        self.text_dict[iid] = (path, name, date)

        self.stats.update_wordlists(word_dict.keys())
        self.stats.set_text_data(path, word_dict)
        percentage = self.stats.get_text_stats(path)

        self.insert(name, date, round(percentage, 3), iid)
    
    def _load_preexisting_items(self):
        text_list = get_csv_files_in_subfolders("D:\Python")
        for path, name, date in text_list:
            iid = self.getNextIid()
            date = date[:-6]
            self.text_dict[iid] = (os_path.dirname(path), name, date)

            self.stats.get_text_data(path)
            percentage = self.stats.get_text_stats(path)

            self.insert(name, date, round(percentage, 3), iid)
        pass
    def getNextIid(self):
        self.current_iid += 1
        return self.current_iid
    def copy_file_path(self):
        iid = int(self.tv.selection()[0])
        path, _, _ = self.text_dict[iid] 

        self.tv.clipboard_clear()
        self.tv.clipboard_append(path)
        pass


def get_csv_files_in_subfolders(directory : str) -> list:
    text_list = list()
    
    # iterate over files in directory
    for foldername in scandir(path=directory):
        if not foldername.is_dir():
            continue
        for filename in scandir(path=foldername):
            if not filename.is_file():
                continue
            csv_ext_index = filename.name.find('.csv')
            if csv_ext_index >=0:
                print(filename.path)
                date = foldername.name
                name = filename.name[:csv_ext_index]
                text_list.append((filename.path, name, date))
    return text_list
