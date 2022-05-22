from configparser import ConfigParser
from pathlib import Path
from enum import Enum
from typing import Dict, Any

class Theme(Enum):
    DEFAULT = 1 #System
    LIGHT = 2
    DARK = 3

    def __str__(self) -> str:
        return str(self.name)

class Language(Enum):
    DEFAULT = 1 #System
    EN = 2 #English
    JA = 3 #Japanese

    def __str__(self) -> str:
        return str(self.name)

class SettingsClass:
    def __init__(self, file_path=str(Path(Path().resolve(), "settings").with_suffix(".ini"))):
        self.file_path = file_path
        self.dict : Dict[str, Dict[str, Any]] = {
            'File': {'Path': Path().resolve()},
            'GUI' : {'Theme': Theme.DEFAULT,
                     'Language': Language.DEFAULT,
                    },
            'Anki' : {'Sync': False,
                     'PreviousSentences': 0,
                     'FollowingSentences': 0,
                     'BoldWord': False,
                     'ItalicWord': False,
                    },
        }
        self.read_ini()

    def set_setting_from_ini(self, section : str, option : str, config : ConfigParser):
        #perform appropriate type conversions for each value
        if isinstance(self.dict[section][option], bool):
            try:
                self.dict[section][option] =\
                    config.getboolean(section=section, option=option, fallback=False)
            except ValueError:
                self.dict[section][option] = False
        elif isinstance(self.dict[section][option], int):
            self.dict[section][option] =\
                config.getint(section=section, option=option, fallback=0)
        elif isinstance(self.dict[section][option], str):
            self.dict[section][option] =\
                config.get(section=section, option=option, fallback="")
        elif isinstance(self.dict[section][option], float):
            self.dict[section][option] =\
                config.getfloat(section=section, option=option, fallback=0.)
        elif isinstance(self.dict[section][option], Path):
            self.get_path(section=section, option=option, config=config)
        elif isinstance(self.dict[section][option], Theme):
            self.get_theme(section=section, option=option, config=config)
        elif isinstance(self.dict[section][option], Language):
            self.get_language(section=section, option=option, config=config)
           
    def get_path(self, section : str, option : str, config : ConfigParser):
        path = Path(config.get(section=section, option=option, fallback=""))
        #Path must be a valid directory
        if path.is_dir():
            #Make the path absolute e.g. '.' becomes current dir
            path = path.resolve()
            self.dict[section][option] = path
    def get_theme(self, section : str, option : str, config : ConfigParser):
        #get theme as a string
        theme_string = config.get(section=section, option=option, fallback="DEFAULT")
        #convert string into a Theme enum value
        match theme_string:
            case "LIGHT":
                self.dict[section][option] = Theme.LIGHT
            case "DARK":
                self.dict[section][option] = Theme.DARK
            case _:
                pass
    def get_language(self, section : str, option : str, config : ConfigParser):
        #get language as a string
        lang_string = config.get(section=section, option=option, fallback="DEFAULT")
        #convert string into a Theme enum value
        match lang_string:
            case "EN":
                self.dict[section][option] = Language.EN
            case "JA":
                self.dict[section][option] = Language.JA
            case _:
                pass
    def read_ini(self):
        #read settings from ini file
        config = ConfigParser()
        config.optionxform=str
        config.read(filenames=self.file_path)

        #update settings values with those from the ini file
        for section in self.dict.keys():
            if config.has_section(section=section):
                for option in self.dict[section].keys():
                        if config.has_option(section=section, option=option):
                            self.set_setting_from_ini(section=section, option=option, config=config)
    def write_ini(self):
        #instantiate ConfigParser object
        config = ConfigParser()
        config.optionxform=str

        #put all settings into ConfigParser object
        for section, options in self.dict.items():
            config[section] = dict()
            for option, value in options.items():
                config[section][option] = str(value)
            # for option in self.dict[section].keys():
            #     config[section][option] = str(self.dict[section][option])

        #write to settings file
        with open(self.file_path, 'w') as configfile:
            config.write(configfile)

def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate

@static_vars(settings=SettingsClass())
def _get_settings_object() -> SettingsClass:
    return _get_settings_object.settings

def get_settings(section : str, option : str) -> Any:
    """
        Returns settings value of ini file's section's option key. 

            Parameters:
                section (str): Name of section in ini
                option (str): Name of key in ini

            Returns:
                result (Any): Settings value of ini file's section's option key
    """
    return _get_settings_object().dict[section][option]

def save_settings() -> None:
    """Save settings to ini file."""
    _get_settings_object().write_ini()

if __name__ == '__main__':
    #SettingsClass()
    pass