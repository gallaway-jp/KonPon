#include "TextParserFunc.h"
#include "MeCab.h"
#include "MecabSlim.h"

#include "Wordlist.h"
#include "Words.h"

#include <string>
#include <map>
#include <set>

#include <QString>
#include <QDir>
#include <QByteArray>

namespace ParserFunc {
    namespace {
        // Return number of bytes of one unicode character
        // input: first byte of unicode character
        inline short GetCharSize(char leadByte) {
            unsigned char uLeadByte = reinterpret_cast<unsigned char&>(leadByte);
            if ((uLeadByte & 0x80) == 0)          // lead bit is zero, must be a single ascii character
                return 1;
            else if ((uLeadByte & 0xE0) == 0xC0)  // 110x xxxx
                return 2;
            else if ((uLeadByte & 0xF0) == 0xE0) // 1110 xxxx
                return 3;
            else if ((uLeadByte & 0xF8) == 0xF0) // 1111 0xxx
                return 4;
            return 0;
        }

        enum FeatureEnum {
            pos1,
            pos2,
            pos3,
            pos4,
            cType,
            cForm,
            lForm,
            lemma,
            orth,
            pron,
            orthBase,
            pronBase,
            goshu,
            iType,
            iForm,
            fType,
            fForm,
            iConType,
            fConType,
            type,
            kana,
            kanaBase,
            form,
            formBase,
            aType,
            aConType,
            aModType,
            lid,
            lemma_id,
            FeatureCount
        };

        inline bool IsJapanesePunctuationCharacter(std::string_view character)
        {
            if (character.length() == 3) {
                //Japanese - style punctuation U + 3000 - U + 303f
                return "\u3000" <= character && character <= "\u303f";
            }
            return false;
        }

        inline bool IsKanaCharacter(std::string_view character)
        {
            if (character.length() == 3) {
                //hiragana U + 3040 - U + 309F
                //katakana U + 30A0 - U + 30FF
                return "\u3040" <= character && character <= "\u30FF";
            }
            return false;
        }

        inline bool IsKatakanaCharacter(std::string_view character)
        {
            if (character.length() == 3) {
                //hiragana U + 3040 - U + 309F
                //katakana U + 30A0 - U + 30FF
                return "\u30A0" <= character && character <= "\u30FF";
            }
            return false;
        }

        unsigned int threeByteUTFToCodePoint(std::string_view character)
        {
            unsigned char highOrder = character[0] & 0x0F;
            unsigned char mediumOrder = character[1] & 0x3F;
            unsigned char lowOrder = character[2] & 0x3F;
            return highOrder << 12 | mediumOrder << 6 | lowOrder;
        }

        std::string codePointToThreeByteUTF(unsigned int codePoint)
        {
            char highOrder = (codePoint & 0xF000) >> 12;
            char mediumOrder = (codePoint & 0x0FC0) >> 6;
            char lowOrder = codePoint & 0x003F;
            char charArray[4] = { 0xE0 | highOrder, 0x80 | mediumOrder, 0x80 | lowOrder, 0x00 };
            return std::string(charArray);
        }

        std::string KatakanaToHiragana(std::string_view string)
        {
            //Converts katanaka characters in string to hiragana characters.
            //
            //Parameters :
            //    string(str) : String to convert
            //
            //    Returns :
            //converted_string(str) : String after conversion
            std::string convertedString = "";
            for (int i = 0; i < string.length(); i++) {
                short size = GetCharSize(string[i]);
                if (size == 0) {
                    size = 1; //unknown character size... add it to the result anyway
                }
                std::string_view character = string.substr(i, size);
                i += size - 1;
                if (IsKatakanaCharacter(character)) {
                    unsigned int codePoint = threeByteUTFToCodePoint(character) - 0x60; //subtract katakana hiragana codepoint difference
                    convertedString += codePointToThreeByteUTF(codePoint); //convert back to UTF8 representation
                    continue;
                }
                convertedString += character;
            }
            return convertedString;
        }

        std::string KatakanaToHiraganaWhenLemmaNotKatakana(std::string_view lemma, std::string_view reading)
        {

            for (int i = 0; i < lemma.length(); i++) {
                short size = GetCharSize(lemma[i]);
                if (size == 3) {
                    if (IsKatakanaCharacter(lemma.substr(i, size))) {
                        return std::string(reading);
                    }
                }
                if (size > 0) {
                    i += size - 1;
                }
            }
            return KatakanaToHiragana(reading);
        }

        bool IsAsciiCharacter(char character)
        {
            //0 - 9 (30 - 39)
            //A - Z(41 - 5A)
            return 0x00 <= character && character <= 0x7F;
        }

        bool IsStrangeCharacter(std::string_view character)
        {
            //Full - width roman characters and half - width katakana(ff00 - ffef)
            return "\uff00" <= character && character <= "\uffef";
        }

        bool IsKanjiCharacter(std::string_view character)
        {
            //CJK unified ideographs Extension A - Rare kanji(3400 - 4dbf)
            //CJK unifed ideographs - Common and uncommon kanji(4e00 - 9faf)
            //CJK unified ideographs Extension B - Very rare kanji(20000-2A6DF)
            return "\u4e00" <= character && character <= "\u9faf"
                || "\u3400" <= character && character <= "\u4dbf"
                || "\u20000" <= character && character <= "\u2A6DF";
        }

        inline bool IsSingleNonKanjiCharacter(std::string_view word) {
            short size = GetCharSize(word[0]);
            if (size > 0) {
                if (size < word.length()) {
                    return false; //If is multiple characters
                }
                std::string_view character = word.substr(0, size);
                if (size >= 3 && IsKanjiCharacter(character)) {
                    return false; //If is kanji character
                }
            }
            return true;
        }

        bool IsAsciiWord(std::string_view word) {
            for (int i = 0; i < word.length(); i++) {
                short size = GetCharSize(word[i]);
                if (size == 1) {
                    continue; //Ascii characters are only 1 byte
                }
                if (size == 3) {
                    std::string_view character = word.substr(i, size);
                    if (IsStrangeCharacter(character)) {
                        i += size - 1;
                        continue;
                    }
                }
                return false;
            }
            return true;
        }

        auto _SplitFeatures(std::string_view str)
        {
            std::vector<std::string_view> features;

            int indexCommaToLeftOfColumn = 0;
            int indexCommaToRightOfColumn = -1;

            bool ignoreDelimiter = false;
            for (int i = 0; i < static_cast<int>(str.size()); i++)
            {
                switch (str[i])
                {
                case ',':
                {
                    if (!ignoreDelimiter) {
                        indexCommaToLeftOfColumn = indexCommaToRightOfColumn;
                        indexCommaToRightOfColumn = i;
                        int index = indexCommaToLeftOfColumn + 1;
                        int length = indexCommaToRightOfColumn - index;

                        std::string_view column(str.data() + index, length);
                        features.push_back(column);
                    }
                    break;
                }
                case '\"':
                    ignoreDelimiter = !ignoreDelimiter;
                    break;
                default:
                    break;
                }
            }
            const std::string_view finalColumn(str.data() + indexCommaToRightOfColumn + 1, str.size() - indexCommaToRightOfColumn - 1);
            features.push_back(finalColumn);

            return features;
        }



        std::string_view trim(std::string_view string, const char* trimCharacterList = " \t\v\r\n")
        {
            std::string_view result;
            std::string::size_type left = string.find_first_not_of(trimCharacterList);

            if (left != std::string::npos)
            {
                std::string::size_type right = string.find_last_not_of(trimCharacterList);
                result = string.substr(left, right - left + 1);
            }
            return result;
        }

        std::vector<uint8_t> accentSplit(std::string_view str)
        {
            std::vector<uint8_t> result;

            int indexCommaToLeftOfColumn = 0;
            int indexCommaToRightOfColumn = -1;

            bool ignoreDelimiter = false;
            for (int i = 0; i < static_cast<int>(str.size()); i++)
            {
                switch (str[i])
                {
                case ',':
                {
                    indexCommaToLeftOfColumn = indexCommaToRightOfColumn;
                    indexCommaToRightOfColumn = i;
                    int index = indexCommaToLeftOfColumn + 1;
                    int length = indexCommaToRightOfColumn - index;

                    std::string_view column(str.data() + index, length);
                    result.push_back(std::stoi(std::string(column)));
                    break;
                }
                default:
                    break;
                }
            }
            const std::string_view finalColumn(str.data() + indexCommaToRightOfColumn + 1, str.size() - indexCommaToRightOfColumn - 1);
            if (!finalColumn.empty()) {
                result.push_back(std::stoi(std::string(finalColumn)));
            }

            return result;
        }

        // Returns character count starting from index in str until length bytes
        size_t getCharacterCount(const char* str, size_t index, size_t length)
        {
            size_t count = 0;
            const char* p = &str[index];
            for (int i = 0; i < length; ++i) {
                // count only the first byte of each character
                count += ((p[i] & 0xc0) != 0x80);
            }
            return count;
        }
    }

    std::pair<WordDict, LocationsDict> Tokenize(std::string const& folder, const char* pInputText, const std::string& fileId, Wordlist& outWordlist, Words& outWords) {
        QByteArray dictionaryPathBA = QDir(folder.c_str()).absoluteFilePath("DictionaryData").toUtf8();

        MecabSlim mcs = MecabSlim(dictionaryPathBA.data());

        auto results = mcs.parse(pInputText);

        int64_t textId = std::stoll(fileId);

        WordDict wordDictionary;
        LocationsDict locationsDictionary;

        auto current = results;
        size_t index = 0;
        size_t charIndex = 0;
        while (current) {
            if (current->length == 0) {
                current = current->next;
                continue;
            }
            auto features = _SplitFeatures(current->feature);
            if (features.size() != FeatureEnum::FeatureCount) {
                current = current->next;
                continue; //invalid number of features, so go to next node
            }
            std::string_view word = features[FeatureEnum::orthBase]; //dictionary form

            if (features[FeatureEnum::orthBase].empty()) {
                word = current->surface;
            }
            if (word.empty()) {
                current = current->next;
                continue;
            }

            if (IsSingleNonKanjiCharacter(word) || IsAsciiWord(word)) {
                current = current->next;
                continue;
            }

            std::string kana = KatakanaToHiraganaWhenLemmaNotKatakana(word, trim(features[kanaBase], "'"));
            if (kana == "") { //dictionary form reading
                current = current->next;
                continue;
            }

            std::string_view accent = features[aType] != "*" ? features[aType] : ""; //pitch accent of word

            std::vector<uint8_t> accents = accentSplit(trim(accent, "\""));

            uint8_t surfaceLen = getCharacterCount(pInputText, 0, current->length); //length of the surface form in text

            //surface is form as appears in text
            size_t oldIndex = index;
            index = std::string_view(pInputText).find(current->surface, index);
            charIndex += getCharacterCount(pInputText, oldIndex, index - oldIndex);

            if (wordDictionary.find(kana) == wordDictionary.end() || wordDictionary[kana].find(std::string(word)) == wordDictionary[kana].end()) {
                Word newWord = Word(kana, std::string(word));
                newWord.addTextId(textId);
                for (uint8_t accent : accents) {
                    newWord.addPitchAccent(accent);
                }
                outWords.insertWord(newWord);
            }
            wordDictionary[kana][std::string(word)][charIndex] = surfaceLen;
            outWordlist.insertWord(kana, std::string(word));
            locationsDictionary[charIndex][kana].insert(std::string(word));

            current = current->next;
        }
        return { wordDictionary, locationsDictionary };
    }
}