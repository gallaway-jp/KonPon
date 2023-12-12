#pragma once
#include <QDialog>

#include "Words.h"

#include <string>
#include <unordered_map>

QT_BEGIN_NAMESPACE
class QFormLayout;
class QLabel;
class QLineEdit;
template <typename T>
class QList;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTextEdit;
class QWidget;
QT_END_NAMESPACE

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
    void onRetranslateUI();

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
    QPushButton* m_createAnkiCardButton = nullptr;
    bool mRequiresNotesUpdate = false;
    QLabel* m_kanaLabel = nullptr;
    QLabel* m_kanjiLabel = nullptr;
    QLabel* m_pitchAccentsLabel = nullptr;
    QLabel* m_textsLabel = nullptr;
    QLabel* m_notesLabel = nullptr;
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