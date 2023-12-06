#pragma once
#include <qdialog.h>

#include "Words.h"

#include <string>
#include <unordered_map>

class QWidget;
template <typename T>
class QList;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QFormLayout;

class Settings;
class Wordlists;

class WordView :
    public QDialog
{
    using TextIdsMap = std::unordered_map<int64_t, std::pair<std::string, QDialog*>>;
    using TextIdsListItemMap = std::unordered_map<int64_t, QListWidgetItem*>;
    Q_OBJECT
public:
    WordView(const std::string& kana, const std::string& kanji, Settings* settings, Wordlists& mWordlists, TextIdsMap& textIds);
    ~WordView();
signals:
    void viewTextClicked(int64_t textId);
    void closeDialog();
public slots:
    void onRemoveTextId(int64_t textId);

private:
    QFormLayout* layout;
    Settings* mSettings;
    Wordlists& mWordlists;
    TextIdsMap& mTextIds;
    QListWidget* mTextList = nullptr;
    TextIdsListItemMap mTextListItems;
    Word mTextWord;
    int mNotesRow = 0;
    QPushButton* mShowMoreButton = nullptr;
    QTextEdit* mNotesEdit = nullptr;
    QPushButton* mCopyButton = nullptr;
    bool mRequiresNotesUpdate = false;
private:
    void setDialogProperties();
    void addKanaKanjiLineWidgets(QFormLayout* layout);
    void addPitchAccentsLineWidget(QFormLayout* layout);
    void addTextListWidget(QFormLayout* layout);

private slots:
    void onCreateAnkiCardButton();
    void onCopyWordInfo();
    void onToggledShowMore(bool checked);
    void onTextClicked(QListWidgetItem* item);
    void onNotesTextChanged();
};