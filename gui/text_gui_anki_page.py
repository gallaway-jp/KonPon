import tkinter
from tkinter.ttk import Frame, Button, Combobox, Entry, Label, Treeview

from ini import get_settings
from language import get_ui_string, StringID as ID

from util.anki_connect import get_model_names, get_model_field_names, get_deck_names, create_deck, create_new_notes
from util.string import findCurrentLine, findFollowingLines, findPreviousLines
from util.threading import work_in_background

from text_gui_anki_action_frame import AnkiActionFrame

class AnkiPage(Frame):
    def __init__(self, parent : Frame, controller) -> None:
        super().__init__(master=parent)
        self.parent = parent
        self.controller = controller

        self.words : dict = controller.words
        self.known_words : list = controller.known_words
        self.unknown_words : list = controller.unknown_words
        self.text : str = controller.text

        self.words_to_send = {}
        self.words_to_ignore = {}

        self.is_initialized = False
        self.deck_types_loaded = False
        self.note_types_request_succeeded = False

    def initialize(self) -> None:
        if self.is_initialized:
            return
        self.columnconfigure(index=tuple(range(7)), weight=1)
        self.rowconfigure(index=tuple(range(6)), weight=1)
        self.rowconfigure(index=6, weight=0)
        self.columnconfigure(index=0, weight=0)

        self.ignore_frame = self._add_ignore_frame()
        self.move_frame = self._add_move_frame()
        self.words_frame = self._add_words_frame()

        self.note_frame = self._add_note_frame()     

        self.action_frame = AnkiActionFrame(parent=self, controller=self.controller, textlist=self.controller.words)

        self.is_initialized = True

    def insert_words(self, is_known_words : bool) -> None:
        self.words_treeview.delete(*self.words_treeview.get_children())
        self.words_to_send : dict
        match is_known_words:
            case None:
                #insert all words
                self.words_to_send = self.words
                pass
            case True:
                self.words_to_send = {k: v for k, v in self.words.items() if k in self.known_words}
                #insert known words
                pass
            case False:
                #insert unknown words
                self.words_to_send = {k: v for k, v in self.words.items() if k in self.unknown_words}
                pass
        for (word, indices) in self.words_to_send.items():
            self.words_treeview.insert('', tkinter.END, text=word, iid=word, open=False, values=[len(indices.keys())])
            is_first = True
            for index in indices.keys():
                if is_first:
                    is_first = False
                    continue # details info!!!
                self.words_treeview.insert(word, tkinter.END, text=index, iid=index, open=False)

    def _add_ignore_frame(self):
        frame = Frame(master=self)
        self.ignore_treeview = Treeview(master=frame, columns=('count'), show='tree')
        self.ignore_treeview.pack(fill=tkinter.BOTH, expand=True)
        
        frame.grid(row=0, column=0, padx=5, pady=5, rowspan=2, columnspan=3, sticky=tkinter.NSEW)

    def _add_move_frame(self):
        frame = Frame(master=self)

        frame.columnconfigure(index=tuple(range(4)), weight=1)
        frame.rowconfigure(index=0, weight=0)

        move_to_ignore_button = Button(master=frame, text="↑", command=self.move_to_ignore)
        move_to_ignore_button.grid(row=0, column=0, padx=5, pady=5, rowspan=1, columnspan=1, sticky=tkinter.NSEW)
        move_all_to_ignore_button = Button(master=frame, text="↑↑", command=self.move_all_to_ignore)
        move_all_to_ignore_button.grid(row=0, column=1, padx=5, pady=5, rowspan=1, columnspan=1, sticky=tkinter.NSEW)
        move_to_words_button = Button(master=frame, text="↓", command=self.move_to_words)
        move_to_words_button.grid(row=0, column=2, padx=5, pady=5, rowspan=1, columnspan=1, sticky=tkinter.NSEW)
        move_all_to_words_button = Button(master=frame, text="↓↓", command=self.move_all_to_words)
        move_all_to_words_button.grid(row=0, column=3, padx=5, pady=5, rowspan=1, columnspan=1, sticky=tkinter.NSEW)

        frame.grid(row=2, column=0, padx=5, pady=5, rowspan=1, columnspan=3, sticky=tkinter.NSEW)

    def move_to_ignore(self):
        self._move_words(from_list=self.words_treeview, to_list=self.ignore_treeview,
            from_dict=self.words_to_send, to_dict=self.words_to_ignore)
    def move_all_to_ignore(self):
        self.words_treeview.selection_set(self.words_treeview.get_children())
        self._move_words(from_list=self.words_treeview, to_list=self.ignore_treeview,
            from_dict=self.words_to_send, to_dict=self.words_to_ignore)
    def move_to_words(self):
        self._move_words(from_list=self.ignore_treeview, to_list=self.words_treeview,
            from_dict=self.words_to_ignore, to_dict=self.words_to_send)
    def move_all_to_words(self):
        self.ignore_treeview.selection_set(self.ignore_treeview.get_children())
        self._move_words(from_list=self.ignore_treeview, to_list=self.words_treeview,
            from_dict=self.words_to_ignore, to_dict=self.words_to_send)
    def _move_words(self, from_list : Treeview, to_list : Treeview, from_dict : dict, to_dict : dict):
        words = from_list.selection() # get selected items
        for word in words:
            from_list.delete(word) #Remove word from first treeview
            indices = from_dict.pop(word) #Remove word from first dictionary
            to_dict[word] = indices #Add word to second dictionary
            #Add word to second treeview
            to_list.insert('', tkinter.END, text=word, iid=word, open=False, values=[len(indices.keys())])
            is_first = True
            for index in indices.keys():
                if is_first:
                    is_first = False
                    continue # details info!!!
                to_list.insert(word, tkinter.END, text=index, iid=index, open=False)

    def _add_words_frame(self):
        frame = Frame(master=self)
        self.words_treeview = Treeview(master=frame, columns=('count'), show='tree')
        self.words_treeview.pack(fill=tkinter.BOTH, expand=True)
        
        frame.grid(row=3, column=0, padx=5, pady=5, rowspan=2, columnspan=3, sticky=tkinter.NSEW)

    def _add_note_frame(self) -> Frame:
        frame = Frame(master=self)

        self.columnconfigure(index=tuple(range(4)), weight=1)
        self.rowconfigure(index=tuple(range(3)), weight=1)

        note_type_label = Label(master=frame, text=get_ui_string(ID.TEXT_ANKI_PAGE_NOTE_TYPE))
        note_type_label.grid(row=0, column=0, padx=5, pady=5, rowspan=1,columnspan=1, sticky=tkinter.NSEW)
        self.note_type_combobox = Combobox(master=frame, state="readonly")
        self.note_type_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -foreground #FEFEFE' % self.note_type_combobox)
        self.note_type_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -background #3A3A3A' % self.note_type_combobox)
        self.note_type_combobox.grid(row=0, column=1, padx=5, pady=5, rowspan=1,columnspan=1, sticky=tkinter.NSEW)

        self.note_type_combobox.bind('<<ComboboxSelected>>', self.on_note_type_selected)

        deck_type_label = Label(master=frame, text=get_ui_string(ID.TEXT_ANKI_PAGE_DECK))
        deck_type_label.grid(row=0, column=2, padx=5, pady=5, rowspan=1,columnspan=1, sticky=tkinter.NSEW)
        self.deck_type_combobox = Combobox(master=frame)
        self.deck_type_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -foreground #FEFEFE' % self.deck_type_combobox)
        self.deck_type_combobox.tk.eval(
            '[ttk::combobox::PopdownWindow %s].f.l configure -background #3A3A3A' % self.deck_type_combobox)
        self.deck_type_combobox.grid(row=0, column=3, padx=5, pady=5, rowspan=1,columnspan=1, sticky=tkinter.NSEW)
        #work_in_background(self._add_deck_types())
        self.deck_type_combobox.bind('<Button-1>', self._add_deck_types)
        
        self.fields_frame = FieldsFrame(parent=frame)

        work_in_background(self._add_note_types_and_fields)

        tag_label = Label(master=frame, text=get_ui_string(ID.TEXT_ANKI_PAGE_TAGS))
        tag_label.grid(row=2, column=0, padx=5, pady=5, rowspan=1,columnspan=1, sticky=tkinter.NSEW)
        self.tag_entry = Entry(master=frame)
        self.tag_entry.grid(row=2, column=1, padx=5, pady=5, rowspan=1,columnspan=3, sticky=tkinter.NSEW)

        frame.grid(row=0, column=4, padx=5, pady=5, rowspan=6, columnspan=3, sticky=tkinter.E)

    def send_to_anki(self) -> None:
        deck_name = self.deck_type_combobox.get()
        if create_deck(deck_name):
            fields_names_values = self.fields_frame.get_fields_names_and_values()
            if fields_names_values:
                fields_lists = []
                for word in self.words_to_send.keys():
                    (kana, accent) = self.words_to_send[word]['details']
                    sentences = self._get_sentences(word)

                    fields_list = fields_names_values.copy()
                    field_name : str
                    field_value : str
                    for (field_name, field_value) in fields_names_values:
                        #TODO if escape character used, do not replace
                        #TODO get kana and sentence
                        field_value = field_value.replace("{kanji}", word)
                        field_value = field_value.replace("{kana}", kana)
                        field_value = field_value.replace("{accent}", accent)
                        field_value = field_value.replace("{sentence}", sentences)
                        fields_list.append((field_name, field_value))
                    fields_lists.append(fields_list)
                    #create_new_notes
                #TODO deal with situation where model does not exist
                create_new_notes(deck_name=deck_name, model_name=self.note_type_combobox.get(),
                    fields_lists=fields_lists, tags=self.tag_entry.get().split())
    def _get_sentences(self, word : str) -> str:
        previous_count = get_settings(section="Anki", option="PreviousSentences")
        following_count = get_settings(section="Anki", option="FollowingSentences")
        indices = list(self.words_to_send[word].keys())
        index = indices[1]
        previous_lines = findPreviousLines(string=self.text, index=index, count=previous_count)
        if previous_lines != "":
            previous_lines += '\n'
        current_line = findCurrentLine(string=self.text, index=index)
        following_lines = findFollowingLines(string=self.text, index=index, count=following_count)
        if following_lines != "":
            current_line += '\n'
        return ''.join([previous_lines, current_line, following_lines])

    def on_note_type_selected(self, event : tkinter.Event) -> None:
        child_widgets = self.fields_frame.winfo_children()
        if len(child_widgets) > 0:
            child_widgets[0].destroy()
        self.fields_frame.add_fields(model_name=self.note_type_combobox.get())

    def _add_note_types_and_fields(self) -> None:
        self.note_types = self._add_note_types()
        self.fields_frame.add_fields(model_name=self.note_type_combobox.get())
        #TODO loop through requesting all fields for each note type
        for model in self.note_types.keys():
            self.note_types[model] = get_model_field_names(model_name=model)
        self.fields_frame.update_model_dict(self.note_types)

    def _add_note_types(self) -> dict:
        model_names = get_model_names()
        if model_names:
            self.note_type_combobox['values'] = model_names
            self.note_types_request_succeeded = True
        else: #anki connect request failed, default to add a Basic Note type
            self.note_type_combobox['values'] = ["Basic"]
        self.note_type_combobox.current(0)
        return {name : None for name in model_names}

    def _add_deck_types(self, event : tkinter.Event) -> bool:
        if self.deck_types_loaded:
            return

        request_succeeded = False
        deck_names = get_deck_names()
        if deck_names:
            self.deck_type_combobox['values'] = deck_names
            request_succeeded = True
        self.deck_types_loaded = True

class FieldsFrame(Frame):
    def __init__(self, parent : Frame) -> None:
        super().__init__(master=parent)

        self.grid(row=1, column=0, padx=5, pady=5, rowspan=1,columnspan=4, sticky=tkinter.NSEW)

        self.fields_names_and_values_widgets = []

        self.note_types = dict()

    def add_fields(self, model_name : str) -> Frame:
        frame = Frame(master=self)
        frame.pack(fill=tkinter.BOTH, expand=True)
        
        self.fields_names_and_values_widgets = []
        field_names = []
        if len(self.note_types) > 0 and self.note_types[model_name] is not None:
            field_names = self.note_types[model_name]
        else:
            field_names = get_model_field_names(model_name=model_name)
        if field_names:
            for field_name in field_names:
                field_label = Label(master=frame, text=field_name)
                field_label.pack(fill=tkinter.BOTH, side=tkinter.TOP)
                field_entry = Entry(master=frame)
                field_entry.pack(fill=tkinter.BOTH, side=tkinter.TOP)

                self.fields_names_and_values_widgets.append((field_label,field_entry))
        else:
            error_string_1 = get_ui_string(ID.TEXT_ANKI_PAGE_FIELDS_RETRIEVAL_FAILED_1)
            error_string_2 = get_ui_string(ID.TEXT_ANKI_PAGE_FIELDS_RETRIEVAL_FAILED_2)
            field_label = Label(master=frame, text=f"{error_string_1}{model_name}{error_string_2}")
            field_label.pack(fill=tkinter.BOTH, expand=True)
        return frame

    def get_fields_names_and_values(self) -> list | None:
        names_and_values_list = []
        if len(self.fields_names_and_values_widgets) <= 0:
            return None
        field_label : Label
        field_entry : Entry
        for (field_label, field_entry) in self.fields_names_and_values_widgets:
            field_name : str = field_label.cget("text")
            field_value : str = field_entry.get()
            names_and_values_list.append((field_name, field_value))
        return names_and_values_list
    
    def update_model_dict(self, note_types : dict) -> None:
        self.note_types = note_types
