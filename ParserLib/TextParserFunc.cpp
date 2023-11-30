#include "TextParserFunc.h"
#include "MeCab.h"
#include "MecabSlim.h"

#include "Wordlist.h"
#include "Words.h"

#include <string>
#include <map>
#include <set>
#include <chrono>
#include <filesystem>

#include <locale>
#include <codecvt>

#include <QString>
#include <QDir>
#include <QByteArray>

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

using Features = std::vector<std::string>;

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

inline bool IsJapanesePunctuationCharacter(const std::string& character)
{
    if (character.length() == 3) {
        //Japanese - style punctuation U + 3000 - U + 303f
        return "\u3000" <= character && character <= "\u303f";
    }
    return false;
}

inline bool IsKanaCharacter(const std::string& character)
{
    if (character.length() == 3) {
        //hiragana U + 3040 - U + 309F
        //katakana U + 30A0 - U + 30FF
        return "\u3040" <= character && character <= "\u30FF";
    }
    return false;
}

inline bool IsKatakanaCharacter(const std::string& character)
{
    if (character.length() == 3) {
        //hiragana U + 3040 - U + 309F
        //katakana U + 30A0 - U + 30FF
        return "\u30A0" <= character && character <= "\u30FF";
    }
    return false;
}

unsigned int threeByteUTFToCodePoint(const std::string& character)
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

std::string KatakanaToHiragana(const std::string& string)
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
        std::string character = string.substr(i, size);
        i += size - 1;
        if (IsKatakanaCharacter(character)) {
            unsigned int codePoint = threeByteUTFToCodePoint(character) - 0x60; //subtract katakana hiragana codepoint difference
            convertedString += codePointToThreeByteUTF(codePoint); //convert back to UTF8 representation
            //convertedString += character[0];
            //
            //unsigned char subtractFromThird = (unsigned short)0x0120 - 0x0100; //= 0x20
            //unsigned short carry = 0x0000;
            //character[1] -= 1; //The difference is more than 1 byte, so subtract at least one from 2nd byte
            //if (character[2] < subtractFromThird) {
            //    character[1] -= 1; //Since third byte's value is not enough, borrow from 2nd byte
            //    carry = 0x0100;
            //}
            //character[2] = carry + character[2] - subtractFromThird;

            //convertedString += character[1];
            //convertedString += character[2];
            continue;
        }
        convertedString += character;
    }
    return convertedString;
}

std::string KatakanaToHiraganaWhenLemmaNotKatakana(std::string lemma, std::string reading)
{
    
    for(int i = 0; i < lemma.length(); i++){
        short size = GetCharSize(lemma[i]);
        if (size == 3) {
            if (IsKatakanaCharacter(lemma.substr(i, size))) {
                return reading;
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

bool IsStrangeCharacter(std::string& character)
{
    //Full - width roman characters and half - width katakana(ff00 - ffef)
    return "\uff00" <= character && character <= "\uffef";
}

bool IsKanjiCharacter(std::string& character)
{
    //CJK unified ideographs Extension A - Rare kanji(3400 - 4dbf)
    //CJK unifed ideographs - Common and uncommon kanji(4e00 - 9faf)
    //CJK unified ideographs Extension B - Very rare kanji(20000-2A6DF)
    return "\u4e00" <= character && character <= "\u9faf"
        || "\u3400" <= character && character <= "\u4dbf"
        || "\u20000" <= character && character <= "\u2A6DF";
}

inline bool IsSingleNonKanjiCharacter(std::string word) {
    short size = GetCharSize(word[0]);
    if (size > 0) {
        if (size < word.length()) {
            return false; //If is multiple characters
        }
        std::string character = word.substr(0, size);
        if (size >=3 && IsKanjiCharacter(character)) {
            return false; //If is kanji character
        }
    }
    return true;
}

bool IsAsciiWord(std::string word) {
    for (int i = 0; i < word.length(); i++) {
        short size = GetCharSize(word[i]);
        if (size == 1) {
            continue; //Ascii characters are only 1 byte
        }
        if (size == 3) {
            std::string character = word.substr(i, size);
            if (IsStrangeCharacter(character)) {
                i += size - 1;
                continue;
            }
        }
        return false;
    }
    return true;
}

Features _SplitFeatures(const std::string& featuresString)
{
    Features features;

    std::string feature;
    for (size_t i = 0; i < featuresString.length(); i++) {
        if (featuresString[i] == ',') {
            features.push_back(feature);
            feature = "";
            continue;
        }
        else if (featuresString[i] == '\"') {
            i++;
            size_t end = featuresString.find('\"', i);
            if (end == std::string::npos) {
                feature.append(featuresString, i);
                break;
            }
            size_t length = end - i;
            if (length <= 0) {
                continue;
            }
            feature.append(featuresString, i, length);
            i += length;
            continue;
        }
        feature.append(1, featuresString[i]);
    }
    if (!feature.empty()) {
        features.push_back(feature);
    }
    return features;
}



std::string trim(const std::string& string, const char* trimCharacterList = " \t\v\r\n")
{
    std::string result;
    std::string::size_type left = string.find_first_not_of(trimCharacterList);

    if (left != std::string::npos)
    {
        std::string::size_type right = string.find_last_not_of(trimCharacterList);
        result = string.substr(left, right - left + 1);
    }
    return result;
}

std::vector<std::string> split(const std::string& string, const char* splitCharacterList = " \t\v\r\n")
{
    std::vector<std::string> result;
    std::string current;
    for (int i = 0; i < string.length(); i++) {
        bool splitCharFound = false;
        const char* pCurrentSplitCharacter = splitCharacterList;
        while(*pCurrentSplitCharacter != 0x00) {
            if (string[i] == *pCurrentSplitCharacter) {
                result.push_back(current);
                current = "";
                splitCharFound = true;
                break;
            }
            pCurrentSplitCharacter++;
        }
        if (!splitCharFound) {
            current += string[i];
        }
    }
    if (!current.empty()) {
        result.push_back(current);
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
        Features features = _SplitFeatures(current->feature);
        if (features.size() != FeatureEnum::FeatureCount) {
            current = current->next;
            continue; //invalid number of features, so go to next node
        }
        std::string word = features[FeatureEnum::orthBase]; //dictionary form
        if (word.empty()) {
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

        std::string accent = features[aType] != "*" ? features[aType] : ""; //pitch accent of word

        std::vector<std::string> accentSplit = split(accent, ",");
        std::vector<uint8_t> accents;
        for (std::string accent : accentSplit) {
            accents.push_back(std::stoi(accent));
        }

        uint8_t surfaceLen = getCharacterCount(pInputText, 0,current->length); //length of the surface form in text
        
        //surface is form as appears in text
        size_t oldIndex = index;
        index = std::string(pInputText).find(current->surface, index);
        charIndex += getCharacterCount(pInputText, oldIndex, index - oldIndex);

        if (wordDictionary.find(kana) == wordDictionary.end() || wordDictionary[kana].find(word) == wordDictionary[kana].end()) {
            Word newWord = Word(kana, word);
            newWord.addTextId(textId);
            for (uint8_t accent : accents) {
                newWord.addPitchAccent(accent);
            }
            outWords.insertWord(newWord);
        }
        wordDictionary[kana][word][charIndex] = surfaceLen;
        outWordlist.insertWord(kana, word);
        locationsDictionary[charIndex][kana].insert(word);

        current = current->next;
    }
    return { wordDictionary, locationsDictionary };
}