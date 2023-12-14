# KonPon
A tool for learning Japanese written in C++
Users can import Japanese texts and keep track of what words are not yet learned, in the process of learning, and already learned.

## Installation
1. Download the zip file under Assets from the latest release found [here](https://github.com/gallaway-jp/KonPon/releases).  
2. Extract to any local folder on your PC and launch KonPon.exe

## Usage
Plain text files (.txt) that contain Japanese text can be imported into KonPon in order to view the text in the app and add words parsed from the text to the unknown word list.  
Words can then be organized into different word lists, such as the unknown and known word lists or into custom-made word lists.  
When viewing a text, words will be highlighted based on their respective word lists' highlight color settings.  
Anki cards can be created from word data by selecting a word either in an open text or from a word list (requires [Anki Connect plugin](https://ankiweb.net/shared/info/2055492159)).

## Building
The following packages are required to build KonPon:  
Microsoft Visual Studio 2022 (Community Edition)  
Qt VS Tools Extension  
Qt 6.6.1  
matrix.bin and sys.dic from [UniDic for Contemporary Written Japanese (ver.3.1.0)](https://clrd.ninjal.ac.jp/unidic/en/back_number_en.html#unidic_bccwj)  
<br>
Follow the below steps to setup the build environment:  
1. Install Visual Studio and install the C++ Desktop Development workload.    
2. Download Qt from [here](https://www.qt.io/download-open-source) and install Qt along with the msvc2019 compiler.  
3. Acquire the Qt VS Tools Extension by launching Visual Studio, opening any solution, open the Extensions Management window from the Extensions menu dropdown, search for Qt VS Tools, and install.  
4. From the Extensions menu in Visual Studio, click QT VS Tools, then click Qt Versions, set the version to 6.6.1, and specify the folder of the msvc2019 compiler.
5. Place matrix.bin and sys.dic in the [unidic folder](MemoryMapLib/unidic/).  

## Other tools

There are other tools available to accomplish similar tasks such as
[Linq](https://www.lingq.com/en/), [Learning with Texts](https://learning-with-texts.sourceforge.io/),
and [jpdb](https://jpdb.io/).

## License and Copyright Notice
The KonPon licence can be found in the LICENSE file.  
<br>
KonPon contains the following software packages used and modified under their respective licenses:  
Unidic - National Institute for Japanese Language and Linguistics, https://clrd.ninjal.ac.jp/unidic/copying/BSD (BSD License)  
MeCab - Taku Kudo `<taku@chasen.org>` and Nippon Telegraph and Telephone Corporation, https://taku910.github.io/mecab/ (BSD License)  
Qt Core - The Qt Company, https://doc.qt.io/qt-6/qtcore-index.html#licenses-and-attributions (LGPL)  
Qt GUI - The Qt Company, https://doc.qt.io/qt-6/qtgui-index.html#licenses-and-attributions (LGPL)  
Qt Test - The Qt Company, https://doc.qt.io/qt-6/qttest-index.html#licenses-and-attributions (LGPL)  
Qt Network - The Qt Company, https://doc.qt.io/qt-6/qtnetwork-index.html#licenses-and-attributions (LGPL)  
Qt Widgets - The Qt Company, https://doc.qt.io/qt-6/qtwidgets-index.html#licenses (LGPL)  
