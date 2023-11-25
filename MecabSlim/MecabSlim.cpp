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

MecabSlim::MecabSlim(const char *pDictionaryPath)
{
    bool bRet = false;

    std::string arg = "";
    std::string system = "mecab-slim";

    arg = system /*+ " -r " + mecabrc*/ + " -d " + pDictionaryPath + arg;

    std::vector<std::string> strings;
    std::istringstream f(arg);
    std::string s;
    while (getline(f, s, ' ')) {
        std::cout << s << std::endl;
        strings.push_back(s);
    }
    int argc = (int)strings.size();
    //char** argv = new char*[argc * sizeof(char*)];
    std::vector<const char*> argv;

    for (int i = 0; i < argc; i++) {
        const char* cstr = (strings.at(i).c_str());
        argv.push_back(cstr);
    }

    mTagger = MeCab::Tagger::create(argc, (char**)argv.data());
    
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



