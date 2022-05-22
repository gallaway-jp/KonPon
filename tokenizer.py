from pathlib import Path
from typing import Tuple, List
from datetime import datetime

import fugashi

from openfile import create_new_folder, copy_file, get_texts, open_text
from util.threading import work_multiple_return_values

#word.feature is a namedtuple of form UnidicFeatures26(pos1, pos2, pos3, pos4, cType, 
#   cForm, lForm, lemma, orth, pron, orthBase, pronBase, goshu, iType, iForm, 
#   fType, fForm, kana, kanaBase, form, formBase, iConType, fConType, aType, aConType, aModeType)

def is_japanese_punctuation_character(character):
    #Japanese-style punctuation U+3000 - U+303f
    return 0x3000 <= ord(character) <= 0x303f

def is_kana_character(character : str) -> bool:
    #hiragana U+3040 - U+309F
    #katakana U+30A0 - U+30FF
    return 0x3040 <= ord(character) <= 0x30FF

def is_katakana_character(character : str) -> bool:
    #hiragana U+3040 - U+309F
    #katakana U+30A0 - U+30FF
    return 0x30A0 <= ord(character) <= 0x30FF

def katakana_to_hiragana(string : str) -> str:
    """
    Converts katanaka characters in string to hiragana characters.

        Parameters:
            string (str): String to convert

        Returns:
            converted_string (str): String after conversion
    """
    converted_string = ""
    for character in string:
        if is_katakana_character(character=character):
            converted_string += chr(ord(character)-0x0060)
            continue
        converted_string += character
    return converted_string

def katakana_to_hiragana_when_lemma_not_katakana(lemma : str, reading : str) -> str:
    for character in lemma:
        if is_katakana_character(character=character):
            return reading
    return katakana_to_hiragana(string=reading)


def is_ascii_character(character):
    #0-9 ( 30 - 39 )
    #A-Z ( 41 - 5A )
    return 0x00 <= ord(character) <= 0x7F

def is_strange_character(character):
    #Full-width roman characters and half-width katakana ( ff00 - ffef)
    return 0xff00 <= ord(character) <= 0xffef

def is_kanji_character(character):
    #CJK unified ideographs Extension A - Rare kanji ( 3400 - 4dbf)
    #CJK unifed ideographs - Common and uncommon kanji ( 4e00 - 9faf)
    return True if 0x4e00 <= ord(character) <= 0x9faf else 0x3400 <= ord(character) <= 0x4dbf

def is_single_non_kanji_character(character):
    if not isinstance(character, str) :
        return False
    return len(character) == 1 and not is_kanji_character(character)

def is_ascii_word(word):
    if not isinstance(word, str) :
        return False
    for character in word:
        if is_strange_character(character) or is_ascii_character(character):
            continue
        return False
    return True

# tagger = fugashi.Tagger()
# #text = "麩を用いた菓子は江戸時代からすでに存在していた。"
# text = open(input("Enter file path for Japanese text to tokenize: "), "r", encoding="utf-8").read()
# word_dictionary = { word.feature.orthBase : 'Test' for word in tagger(text) if not is_single_non_kanji_character(word.feature.orthBase)}
# [print(key) for key in word_dictionary.keys()]

# def tokenize(text) -> dict:
#     word_dictionary = {}

#     #Use fugashi to tokenize text and place words in dictionary
#     tagger = fugashi.Tagger()

#     index = 0
#     for token in tagger(text):
#         word = token.feature.orthBase
#         if word is None:
#             word = token.surface
#         if is_single_non_kanji_character(word) or is_ascii_word(word):
#             continue
#         surface = token.surface
#         surface_len = len(surface)
#         index = text.index(surface, index)

#         if word not in word_dictionary:
#             word_dictionary[word] = []
        
#         word_dictionary[word].append((surface_len,index))
#         index += surface_len 
#     return word_dictionary

def tokenize(text) -> dict:
    word_dictionary = {}

    #Use fugashi to tokenize text and place words in dictionary
    tagger = fugashi.Tagger()

    index = 0
    for token in tagger(text):
        word = token.feature.orthBase #dictionary form

        if word is None:
            word = token.surface

        if is_single_non_kanji_character(word) or is_ascii_word(word):
            continue

        kana = katakana_to_hiragana_when_lemma_not_katakana(word, token.feature.kanaBase.strip("'"))\
            if token.feature.kanaBase is not None else "" #dictionary form reading
        accent = token.feature.aType\
            if token.feature.aType is not None and token.feature.aType != "*" else "" #pitch accent of word
        surface = token.surface #form as appears in text
        surface_len = len(surface) #length of the form in text
        index = text.index(surface, index)

        if word not in word_dictionary:
            word_dictionary[word] = [(kana, accent)]
        
        word_dictionary[word].append((surface_len,index))
        index += surface_len 
    return word_dictionary

def output_to_file(filepath, word_dictionary) -> None:
    with open(filepath, 'w', encoding="utf-8") as file:
        for key, value in word_dictionary.items():
            file.write(f'{key},')
            bool_first = True
            for element in value:
                if(bool_first == True):
                    element = str(element).replace("'","")
                    bool_first = False
                else:
                    file.write(f',')
                file.write(f'{element}')
            file.write(f'\n')

def tokenize_text(folder : str, name : str, text : str, results_directory : str) -> Tuple[dict, str, str]:
    #tokenize text from file and return dictionary of words; values are lists of (len(surface),<index in text>) tuples
    word_dictionary = tokenize(text)

    time = datetime.utcnow().strftime('%Y%m%d%H%M%S%f')
    
    
    output_folder = str(Path(results_directory, time))
    create_new_folder(path=output_folder)

    output_path_txt = str(Path(output_folder, name).with_suffix(suffix='.txt'))
    output_path_csv = str(Path(output_path_txt).with_suffix(suffix='.csv'))

    #output dictionary to file; each line in file is a key plus comma followed by the elements in the value separated by commas
    output_to_file(output_path_csv, word_dictionary)
    #copy original text file to the folder used by KonPon for viewing
    copy_file(str(Path(folder, name)), output_path_txt)

    return word_dictionary, time, output_folder

def get_text_and_tokenize(filepath : str) -> Tuple[str, str, str, dict]:
    """Retrieves  and tokenizes selected texts."""
    folder, filename, text = open_text(filepath)
    word_dictionary, date, output_path = tokenize_text(folder, filename, text, "D:\Python")
    return filename, date, output_path, word_dictionary

def open_and_tokenize_texts() -> List[Tuple[str, str, str, dict]] | None:
        """Shows open file dialog and tokenizes selected texts."""
        filepaths = get_texts()
        if not filepaths:
            return None
        return work_multiple_return_values([get_text_and_tokenize]*len(filepaths),
            [(filepath,) for filepath in filepaths])