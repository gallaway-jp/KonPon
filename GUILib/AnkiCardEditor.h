#pragma once
#include <QWidget>

#include <string>
#include <vector>
#include <unordered_set>

#include <QTextEdit>
#include <QVBoxLayout>
#include <QFormLayout>

#include <QMap>

class QToolBar;
class QPushButton;
class QScrollArea;
class QLineEdit;

class AnkiCardEditor :
    public QWidget
{
public:
    AnkiCardEditor();
    QMap<QString, QString> getFields();
    QStringList getTags();
    QString getCurrentNoteType(bool& isNewNoteType);
    QString getCurrentDeck(bool& isNewDeck);
public slots:
    void onUpdateDeckNames(const QStringList& decks);
    void onUpdateNoteTypes(const QStringList& models, const QList<QStringList>& fieldsLists);
    void onInsertDataIntoField(const QString& data, const QString& field);
private:
    void setTextWidth();
    void createNoteDeckButtons();
    void createFields();
    QToolBar* createEditToolbar();
    void setFields(const QString& noteType);
    void setDeck(const QString& deck);
    int setHeight(QTextEdit* ptxt, int nRows);
private:
    QString mCurrentNoteType = "Default";
    QString mCurrentDeck = "Default";
    int mTextWidth = 0;
    int fieldHeight = 0;
    QScrollArea* scrollArea = nullptr;
    QVBoxLayout* mLayout = nullptr;
    QFormLayout* fieldsLayout = nullptr;
    QLineEdit* mTagEdit = nullptr;
    QPushButton* mNoteTypeButton = nullptr;
    QPushButton* mDeckButton = nullptr;
    //std::vector<std::pair<std::string, QTextEdit*>> mFields;
    QStringList mDecks = { "Default" };
    QMap<QString, QStringList> mNoteTypes = { {"Default", {"Front", "Back"}}};
    std::unordered_set <QString> mNewNoteTypes;
    std::unordered_set <QString> mNewDecks;

private slots:
    void onBoldActionToggled(bool checked);
    void onItalicActionToggled(bool checked);
    void onUnderlineActionToggled(bool checked);
    void onStrikeoutActionToggled(bool checked);
    void onColorActionTriggered(bool checked);
    void onHighlightActionTriggered(bool checked);
    void onNoteTypeButtonClicked();
    void onDeckButtonClicked();
};