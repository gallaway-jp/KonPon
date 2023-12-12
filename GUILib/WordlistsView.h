#pragma once
#include <qdialog.h>

#include <string>
#include <map>

class TextTree;
class Settings;
class Wordlists;

class QWidget;
template <typename T>
class QList;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QComboBox;
class QStackedWidget;
class QListWidget;
class QListWidgetItem;
#include "Wordlists.h"

class WordlistsView :
    public QDialog
{
    Q_OBJECT
public:
    WordlistsView(QWidget* parent, TextTree* textTree, Settings* settings, Wordlists& wordlists);
    ~WordlistsView();
private:
    void createLists();
    void createConnects();
    void setComboboxConnects(bool setConnect = true);
private:
    TextTree* mTextTree;
    Settings* mSettings = nullptr;
    Wordlists& mWordlists;

    QComboBox* mLeftCombobox = nullptr;
    QComboBox* mRightCombobox = nullptr;
    QStackedWidget* mLeftStack = nullptr;
    QStackedWidget* mRightStack = nullptr;
    QListWidget* mUnknownList = nullptr;
    QListWidget* mKnownList = nullptr;
    std::map<std::string, QListWidget*> mCustomLists;
    QPushButton* mMoveRightButton = nullptr;
    QPushButton* mMoveLeftButton = nullptr;
    QPushButton* mAddCustomListButton = nullptr;
    QPushButton* mDeleteCustomListButton = nullptr;
    std::string mPrevLeftItem = "";
    std::string mPrevRightItem = "";
    QPushButton* mLeftListEditButton = nullptr;
    QPushButton* mRightListEditButton = nullptr;
    QPushButton* mLeftAddWordButton = nullptr;
    QPushButton* mLeftDeleteWordsButton = nullptr;
    QPushButton* mRightAddWordButton = nullptr;
    QPushButton* mRightDeleteWordsButton = nullptr;
signals:
    void viewWordClicked(const std::string& kana, const std::string& kanji);
    void closeWordViewWindow(const std::string& kana, const std::string& kanji);
public slots:
    void onLeftChange(int index);
    void onRightChange(int index);
    void onAddCustomList();
    void onDeleteCustomList();
    void deleteFromCombobox(QComboBox* combobox, const std::string& itemText);
    void onMoveRight();
    void onMoveLeft();
    void onMove(QComboBox* sourceCombobox, QComboBox* destCombobox, QStackedWidget* sourceStack, QStackedWidget* destStack);
    void onEditLeftList();
    void onEditRightList();
    void onEditList(QComboBox* combobox);
    void onReloadWordlists();
    void onRetranslateUI();
private slots:
    void onAddLeftWord();
    void onAddRightWord();
    void onDeleteLeftWords();
    void onDeleteRightWords();
private:
    void onComboboxChange(QComboBox* activeCombobox, QComboBox* inactiveCombobox, std::string& previousItem, int index);
    void setCurrentList(QStackedWidget* stack, QComboBox* combobox);
    void setItemColor(QComboBox* combobox, int index, WordListInfo::Color color);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onAddWord(QListWidget* list, WordListInfo::Type type, const std::string& listName);
    void onDeleteWords(QListWidget* list, WordListInfo::Type type, const std::string& listName = "");
    void insertWordsFromUnknownWordlistIntoListWidget();
    void insertWordsFromKnownWordlistIntoListWidget();
    void insertWordsFromCustomWordlistIntoListWidget(const std::string& customListName);
};