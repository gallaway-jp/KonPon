import json
from typing import Any
from urllib.error import URLError
import urllib.request
from http.client import HTTPResponse

def format_request(action : str, **params) -> str:
    request = {'action': action, 'params': params, 'version': 6}
    return json.dumps(request).encode('utf-8')

def do_request(request : str) -> dict:
    http_response  : HTTPResponse
    try:
        http_response  = urllib.request.urlopen(
        urllib.request.Request(url='http://localhost:8765', data=request))
    except URLError:
        pass
    else:
        response : dict = json.load(http_response)
        return response
    return None
def invoke(action : str, **params) -> Any:
    response : dict = do_request(format_request(action, **params))
    #response = json.load(urllib.request.urlopen(urllib.request.Request('http://localhost:8765', requestJson)))
    if response is None:
        raise Exception('request has failed')
    if len(response) != 2:
        raise Exception('response has an unexpected number of fields')
    if 'error' not in response:
        raise Exception('response is missing required error field')
    if 'result' not in response:
        raise Exception('response is missing required result field')
    if response['error'] is not None:
        raise Exception(response['error'])
    return response['result']

def deck_exists(deck_name) -> bool | None:
    decks : list
    try:
        decks = invoke('deckNames')
    except:
        return None
    else:
        if deck_name in decks:
            return True
    return False

def get_deck_names() -> list | None:
    decks : list
    try:
        decks = invoke('deckNames')
    except:
        return None
    else:
        return decks

def create_new_deck(deck_name : str) -> bool:
    if deck_exists(deck_name=deck_name) is not False:
        #TODO show dialog telling user cannot make new deck
        return False
    try:
        invoke('createDeck', deck=deck_name)
    except:
        return False
    return True

def create_deck(deck_name : str) -> bool:
    try:
        invoke('createDeck', deck=deck_name)
    except:
        return False
    return True

def get_model_field_names(model_name : str) -> list | None:
    field_names_list : list
    try:
        field_names_list = invoke('modelFieldNames', modelName=model_name)
    except:
        return None
    else:
        return field_names_list

def get_model_names() -> list | None:
    field_names_list : list
    try:
        field_names_list = invoke('modelNames')
    except:
        return None
    else:
        return field_names_list


class AnkiNote():
    """
    "note": {
        "deckName": "Default",
        "modelName": "Basic",
        "fields": {
            "Front": "front content",
            "Back": "back content"
        },
        "options": {
            "allowDuplicate": false,
            "duplicateScope": "deck",
            "duplicateScopeOptions": {
                "deckName": "Default",
                "checkChildren": false,
                "checkAllModels": false
            }
        },
        "tags": [
            "yomichan"
        ],
        "audio": [{
            "url": "https://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji=猫&kana=ねこ",
            "filename": "yomichan_ねこ_猫.mp3",
            "skipHash": "7e2c2f954ef6051373ba916f000168dc",
            "fields": [
                "Front"
            ]
        }],
        "video": [{
            "url": "https://cdn.videvo.net/videvo_files/video/free/2015-06/small_watermarked/Contador_Glam_preview.mp4",
            "filename": "countdown.mp4",
            "skipHash": "4117e8aab0d37534d9c8eac362388bbe",
            "fields": [
                "Back"
            ]
        }],
        "picture": [{
            "url": "https://upload.wikimedia.org/wikipedia/commons/thumb/c/c7/A_black_cat_named_Tilly.jpg/220px-A_black_cat_named_Tilly.jpg",
            "filename": "black_cat.jpg",
            "skipHash": "8d6e4646dfae812bf39651b59d7429ce",
            "fields": [
                "Back"
            ]
        }]
    }
    """
    def __init__(self, deck_name : str, fields_names_values : list, model_name : str = "Basic", tags : list = []) -> None:
        self.deck_name = deck_name
        self.model_name = model_name
        self.fields_names_values = dict(fields_names_values)
        #self.fields = self._make_fields(values=fields_values)
        self.options = self._make_options()
        self.tags = tags
        # self.audio = audio
        # self.vidio = video
        # self.picture = picture

        self._dictionary = self._make_dictionary()
    
    def _make_fields(self, values : list) -> dict:
        if self.model_name == "Basic":
            names = ["Front", "Back"]
        else:
            names = get_model_field_names(self.model_name)
        if not names:
            return {}
        return dict(zip(names, values))
    
    def _make_options(self) -> dict:
        options_dict = dict()
        options_dict["allowDuplicate"] = False
        options_dict["duplicateScope"] = "deck"
        options_dict["duplicateScopeOptions"] = {
            "deckName": None,
            "checkChildren": False,
            "checkAllModels": False
        }
        
        return options_dict

    def _make_dictionary(self) -> dict:
        note_dict = dict()
        note_dict["deckName"] = self.deck_name
        note_dict["modelName"] = self.model_name
        note_dict["fields"] = self.fields_names_values
        note_dict["options"] = self.options
        note_dict["tags"] = self.tags

        return note_dict
    
    def get_dict(self) -> dict:
        return self._dictionary
    
    def __str__(self) -> str:
        return str(self._dictionary)

from typing import List, NamedTuple
class NoteField(NamedTuple):
    name : str
    value : str

def create_new_note(deck_name : str, model_name : str, fields_names_values : List[NoteField], tags : list) -> bool:
    anki_note = AnkiNote(deck_name=deck_name, model_name=model_name, fields_names_values=fields_names_values, tags=tags)
    note_dict = anki_note.get_dict()
    try:
        invoke('addNote', note=note_dict)
    except:
        return False
    return True

def create_new_notes(deck_name : str, model_name : str, fields_lists : List[List[NoteField]], tags : list) -> bool:
    notes_list = []
    for field_list in fields_lists:
        anki_note = AnkiNote(deck_name=deck_name, model_name=model_name, fields_names_values=field_list, tags=tags)
        notes_list.append(anki_note.get_dict())
    try:
        invoke('addNotes', notes=notes_list)
    except:
        return False
    return True


#create_new_deck(deck_name='test1')
#create_new_note(deck_name='test1', anki_note=AnkiNote(deck_name="test1", model_name="基本-3e081",fields_values=["aa","bb"]))
