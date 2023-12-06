#pragma once

#include <QWidget>
class TextTree;
class Settings;
class Wordlists;
class WordlistsView;

class BottomMenu : public QWidget
{
    Q_OBJECT
public:
    BottomMenu(QWidget* parent, TextTree* textTree, Settings* settings, Wordlists& wordlists);
private:
    TextTree* mTextTree;
    Settings* _mSettings;
    Wordlists& mWordlists;
    WordlistsView* mWordlistsView = nullptr;
private:
    void onAddFolderClicked();
    void onAddTextClicked();
    void onAddAudioClicked();
    void onAddTextAudioClicked();
    void onDeleteFolderClicked();
    void onDeleteFileClicked();
    void onViewTextClicked();
    void onViewWordlistsClicked();
signals:
    void tokenizeText(const QString& workspace);
    void deleteFolderClicked();
    void deleteFileClicked();
    void viewTextClicked();
    void viewWordClicked(const std::string& kana, const std::string& kanji);
    void closeWordViewWindow(const std::string& kana, const std::string& kanji);
};