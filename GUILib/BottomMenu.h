#pragma once

#include <QWidget>
class TextTree;
class Settings;
class Wordlists;
class WordlistsView;

QT_BEGIN_NAMESPACE
class QAction;
class QPushButton;
QT_END_NAMESPACE

class BottomMenu : public QWidget
{
    Q_OBJECT
public:
    BottomMenu(QWidget* parent, TextTree* textTree, Settings* settings, Wordlists& wordlists);
    ~BottomMenu();
private:
    TextTree* mTextTree;
    Settings* _mSettings;
    Wordlists& mWordlists;
    WordlistsView* mWordlistsView = nullptr;
private:
    QPushButton* m_addFolderButton = nullptr;
    QToolButton* m_addTextButton = nullptr;
    QAction* m_addAudioAction = nullptr;
    QAction* m_addTextAudioAction = nullptr;
    QPushButton* m_deleteFolderButton = nullptr;
    QPushButton* m_deleteTextButton = nullptr;
    QPushButton* m_viewTextButton = nullptr;
    QPushButton* m_viewWordlistsButton = nullptr;
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
    void textsTokenized();
    void retranslateUI();
public slots:
    void onRetranslateUI();
};