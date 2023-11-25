#pragma once
//namespace MeCab
//{
//    class Tagger;
//    typedef struct mecab_node_t Node;
//};
#include "mecab.h"

class MecabSlim
{
private:
    MeCab::Tagger *mTagger;
public:
    MecabSlim(const char* pDictionaryPath);
    ~MecabSlim();

    const MeCab::Node* parse(const char* pText);
};