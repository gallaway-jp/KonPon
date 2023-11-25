#include "WordlistTest.h"
#include "Wordlist.h"
#include "Word.h"

QTEST_MAIN(WordlistTest)
//#include "WordlistTest.moc"

#include <map>
#include <string>

#include <QString>
#include <QDebug>

void WordlistTest::addWords()
{
    Wordlist list = Wordlist();
    list.insertWord("kana1", "kanji1b");
    list.insertWord("kana1", "kanji1a");
    list.insertWord("kana2", "kanji2");

    list.insertWord("kana1", "kanji1b");
    list.insertWord("kana1", "kanji1aa");
    //qDebug() << "Added from myMap:";
    
    std::string lastKey;
    std::string lastVal;
    bool isFirst = true;
    for (auto const& [key, val] : list){
        //qDebug() << key << ", " << val;
        if (!isFirst) {
            if (key > lastKey) {
                continue;
            }
            else if (key == lastKey && val > lastVal) {
                continue;
            }
            QFAIL("Invalid key, val pair in Wordlist!");
        }
        isFirst = false;
        lastKey = key;
        lastVal = val;
    }
    auto newWords = Wordlist();
    newWords.insertWord("kana1", "kanji1a");
    newWords.insertWord("kana1", "kanji1c");
    newWords.insertWord("kana2", "kanji2");
    list.insertWords(newWords);
    //qDebug() << "Added from myMap2:";
    isFirst = true;
    for (auto const& [key, val] : list) {
        //qDebug() << key << ", " << val;
        if (!isFirst) {
            if (key > lastKey) {
                continue;
            }
            else if (key == lastKey && val > lastVal) {
                continue;
            }
            QFAIL("Invalid key, val pair in Wordlist!");
        }
        isFirst = false;
        lastKey = key;
        lastVal = val;
    }
}

void WordlistTest::removeWords()
{
    Wordlist list = Wordlist();
    list.insertWord("kana1", "kanji1b");
    list.insertWord("kana1", "kanji1a");
    list.insertWord("kana2", "kanji2");
    list.insertWord("kana1", "kanji1b");
    list.insertWord("kana1", "kanji1aa");

    size_t countOfWords = list.size();
    
    Wordlist removeList = Wordlist();
    removeList.insertWord("kana1", "kanji1a");
    removeList.insertWord("kana1", "kanji1c");
    removeList.insertWord("kana2", "kanji2");
    list.removeWords(removeList);
    list.removeWord("kana1", "kanji1b");
    list.removeWord("kana1", "kanji1bbb");

    QVERIFY(list.size() < countOfWords);

    std::string lastKey;
    std::string lastVal;
    bool isFirst = true;
    for (auto const& [key, val] : list) {
        if (!isFirst) {
            if (key > lastKey) {
                continue;
            }
            else if (key == lastKey && val > lastVal) {
                continue;
            }
            QFAIL("Invalid key, val pair in Wordlist!");
        }
        isFirst = false;
        lastKey = key;
        lastVal = val;
    }
}

void WordlistTest::iterateWords()
{
    Wordlist wordlist = Wordlist();
    wordlist.insertWord("kana1", "kanji1b");
    wordlist.insertWord("kana1", "kanji1a");
    wordlist.insertWord("kana2", "kanji2");
    wordlist.insertWord("kana1", "kanji1b");
    wordlist.insertWord("kana1", "kanji1aa");
    bool isFirst = true;
    std::string lastKey;
    std::string lastVal;
    for (auto const& [key, val] : wordlist) {
        qDebug() << key << ", " << val;
        if (!isFirst) {
            if (key > lastKey) {
                continue;
            }
            else if (key == lastKey && val > lastVal) {
                continue;
            }
            QFAIL("Invalid key, val pair in Wordlist!");
        }
        isFirst = false;
        lastKey = key;
        lastVal = val;
    }
}

void WordlistTest::addWord()
{
    Word wordItem = Word("kana1", "kanji1");
    wordItem.addTextId(123456789);
    wordItem.addTextId(123456789);
    wordItem.addPitchAccent(4);
    wordItem.addPitchAccent(5);
    wordItem.addPitchAccent(4);

    for (int64_t textId : wordItem.getTextIds()) {
        qDebug() << textId;
    }

    for (int8_t pitchAccent : wordItem.getPitchAccents()) {
        qDebug() << pitchAccent;
    }
}