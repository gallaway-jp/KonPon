#pragma once
#include <QtTest/QtTest>
class WordlistTest : public QObject
{
    Q_OBJECT
private slots:
    void addWords();
    void removeWords();
    void iterateWords();

    void addWord();
};