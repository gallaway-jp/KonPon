from contextlib import suppress
from os.path import split as split_path, exists
from os import makedirs
from pathlib import Path
from shutil import copyfile, rmtree
from typing import List, Tuple
from tkinter.filedialog import askdirectory, askopenfilenames

from ini import get_settings

def open_text(filepath : str) -> Tuple[str, str, str]:
    """
    Returns folder name, file name, and text data from file.
    """
    with open(filepath, "r", encoding="utf-8") as input_file:
        folder,filename = split_path(input_file.name)
        text = input_file.read()
    return folder, filename, text

def get_texts() -> List[str] | None:
    """Returns list of file paths selected from open file dialog."""
    filepaths = askopenfilenames(
        filetypes=[("Text Files", "*.txt")]
    )
    if not filepaths:
        return None
    #Use multiple threads to open file and get folder name, file name, and text data
    #return work_multiple_with_return_values([lambda : open_text(filepath) for filepath in filepaths])
    #a = [open_text, filepath for filepath in filepaths]
    return filepaths

# def get_text():
#     filename = ""
#     text = ""
#     """Open a file for editing."""
#     filepath = askopenfilename(
#         filetypes=[("Text Files", "*.txt")]
#     )
#     if not filepath:
#         return filename, text
#     with open(filepath, "r", encoding="utf-8") as input_file:
#         folder,filename = split_path(input_file.name)
#         text = input_file.read()
#     return folder, filename, text

def get_directory():
    directory = askdirectory()
    if not directory:
        return ""
    return directory

#copy file
def copy_file(src_path : str, dst_path : str) -> None:
    with suppress(Exception):
        copyfile(src=src_path, dst=dst_path, follow_symlinks=False)


#create folder if does not exist
def create_new_folder(path : str) -> None:
    makedirs(name=path, exist_ok = True)

#create file if does not exist
def create_new_file(path):
    with suppress(FileExistsError):
        with open(path, mode='x', encoding='utf-8'):
            pass

def create_word_files():
    directory = get_settings(section="File", option="Path")
    create_new_file(Path(directory, "words", "known_words").with_suffix(".txt"))
    create_new_file(Path(directory, "words", "unknown_words").with_suffix(".txt"))
    #create_new_file(r"D:\Python\Unknown_Words\words.txt")

def write_lines(file_path, lines) -> None:
    with open(file_path, "w", encoding="utf-8") as output_file:
        bool_first = True
        for line in lines:
            if bool_first:
                bool_first = False
            else:
                output_file.write("\n")
            output_file.write(line)

def read_lines(file_path) -> list:
    lines = list()
    with open(file_path, "r", encoding="utf-8") as input_file:
            lines = input_file.read().splitlines()
    return lines

def open_file_as_string(file_path : str) -> str:
    text = str()
    with open(file_path, "r", encoding="utf-8") as input_file:
            text = input_file.read()
    return text

def delete_folder_with_items(path : str):
    if not exists(path):
        return False
    rmtree(path=path, ignore_errors=True)
    return True