from msilib.schema import File
import string
import unittest

from context import ini
class TestIni(unittest.TestCase):
    def test_settings_types(self):
        """
        Test settings object's types
        """
        settings = ini.SettingsClass()
        self.assertIsInstance(settings.dict['File']['Path'], ini.Path, f'Type must be {ini.Path}')
        self.assertIsInstance(settings.dict['GUI']['Theme'], ini.Theme, f'Type must be {ini.Theme}')
        self.assertIsInstance(settings.dict['GUI']['Language'], ini.Language, f'Type must be {ini.Language}')
    
    def test_settings_values(self):
        """
        Test settings object's values
        """
        settings = ini.SettingsClass()
        self.assertEquals(str(settings.dict['File']['Path']), str(ini.Path().resolve()), f'Path should be {str(ini.Path().resolve())}')
        self.assertEquals(str(settings.dict['GUI']['Theme']), 'DEFAULT')
        self.assertEquals(str(settings.dict['GUI']['Language']), 'DEFAULT')


if __name__ == '__main__':
    unittest.main()