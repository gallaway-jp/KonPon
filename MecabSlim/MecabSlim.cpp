// MecabSlim.cpp : スタティック ライブラリ用の関数を定義します。
//
#include "MecabSlim.h"

#include "mecab.h"

//#include "pch.h"
//#include "framework.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

MecabSlim::MecabSlim()
{
    std::vector<const char*> argv = { "mecab-slim" };

    mTagger = MeCab::Tagger::create(1, (char**)argv.data());
    
    if (!mTagger) {
        throw std::runtime_error("Failed to create tagger!");
    }
}

const MeCab::Node *MecabSlim::parse(const char* pText)
{
    return mTagger->parseToNode(pText);
}

MecabSlim::~MecabSlim()
{
    delete mTagger;
}



