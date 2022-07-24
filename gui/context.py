import os
import sys
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from ini import save_settings
from theme import Theme, get_current_theme, set_dark_theme
from language import get_ui_string, StringID
from stats import Stats
from util.openfile import create_word_files