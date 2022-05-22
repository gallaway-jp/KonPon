from pathlib import Path
from ini import get_settings
from openfile import write_lines, read_lines

class Stats():
    def __init__(self):
        self.known_words = list()
        self.unknown_words = list()
        self.text_data_dict = dict()
        self._get_known_words()
        self._get_unknown_words()   

    def update_wordlists(self, word_list : list) -> None:
        for word in word_list:
            if word in self.known_words:
                continue
            if word not in self.unknown_words:
                self.unknown_words.append(word)

    
    def set_text_data(self, path : str, word_dict : dict):
        self.text_data_dict[path] = word_dict

    def get_text_data(self, text_file_path : str) -> dict:
        if text_file_path in self.text_data_dict:
            #already have text data stored in dictionary!!!
            return self.text_data_dict[text_file_path]

        lines = read_lines(file_path=text_file_path)
        
        word_dictionary = dict()
        for line in lines:
            index = line.find(',')
            if index == -1:
                #invalid data
                continue
            word = line[0:index]
            word_dictionary[word] = dict()

            #get (kana, accent) tupple
            index = line.find('(',index)
            if index == -1:
                break
            len_index_start = index+1
            index = line.find(',',index)
            if index == -1:
                break
            len_index_end = index
            index = line.find(')',index)
            if index == -1:
                break
            index_index_end = index
            word_dictionary[word]['details'] = (line[len_index_start:len_index_end], line[len_index_end+1:index_index_end].strip())

            #get (surface_len, index) tupples
            while True:
                index = line.find('(',index)
                if index == -1:
                    break
                len_index_start = index+1
                index = line.find(',',index)
                if index == -1:
                    break
                len_index_end = index
                index = line.find(')',index)
                if index == -1:
                    break
                index_index_end = index
                #add [length of surface] to word's [index in text] dictionary
                word_dictionary[word][int(line[len_index_end+1:index_index_end].strip())] = int(line[len_index_start:len_index_end])
        self.text_data_dict[text_file_path] = word_dictionary
        return word_dictionary
    def get_text_stats(self, text_file_path):
        known_words_count = 0
        total_text_words = 1

        for word in self.known_words:
            if word in self.text_data_dict[text_file_path]:
                known_words_count += 1
            elif word not in self.unknown_words:
                self.unknown_words.append(word)

        total_text_words = len(self.text_data_dict[text_file_path])
        return known_words_count / total_text_words * 100

    def _get_known_words(self):
        directory = get_settings(section="File", option="Path")
        known_file_path = str(Path(directory, "words", "known_words").with_suffix(".txt"))
        self.known_words = read_lines(known_file_path)
    def _get_unknown_words(self):
        directory = get_settings(section="File", option="Path")
        unknown_file_path = str(Path(directory, "words", "unknown_words").with_suffix(".txt"))
        self.unknown_words = read_lines(unknown_file_path)
    
    def save_known_words(self):
        directory = get_settings(section="File", option="Path")
        known_file_path = str(Path(directory, "words", "known_words").with_suffix(".txt"))
        write_lines(file_path=known_file_path, lines=self.known_words)
    
    def save_unknown_words(self):
        directory = get_settings(section="File", option="Path")
        unknown_file_path = str(Path(directory, "words", "unknown_words").with_suffix(".txt"))
        write_lines(file_path=unknown_file_path, lines=self.unknown_words)
                

if __name__ == "__main__":
    pass
