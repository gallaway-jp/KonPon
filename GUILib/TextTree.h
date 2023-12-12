#pragma once

#include "Util.h"
#include "Wordlist.h"
#include "Words.h"

#include <unordered_map>
#include <string>

#include <QElapsedTimer>
#include <QTreeView>

class Settings;
class Wordlists;

QT_BEGIN_NAMESPACE
class QJsonObject;
class QStandardItem;
class QStandardItemModel;
class QString;
class QWidget;
QT_END_NAMESPACE

class TextTree : public QTreeView
{
    using TextIdsMap = std::unordered_map<int64_t, std::pair<std::string, QDialog*>>;
    using WordMap = std::unordered_map <std::pair<std::string, std::string>, QDialog*, WordMapHashFunction>;
    Q_OBJECT
public:
    TextTree(QWidget* parent, Settings* settings, Wordlists* wordlists);
    ~TextTree();

    void addFolder(const QString& folder);
    void removeFolder();
private:
    Settings* _mSettings;
    QStandardItemModel* _mModel;
    QStandardItem* _mRoot;
    bool _mIsSaveNeeded = false;
    Wordlists& mWordlists;
    QElapsedTimer mMousePressedTimer;
private:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

    bool TextTreeReadFromJsonFile();
    void parseElement(const QString& key, const QJsonValue& value, QStandardItem* currentFolder);
    void ParseFolder(const QString& folderName,
        const QJsonObject& folder, QStandardItem* parentFolder);
    bool TextTreeWriteToJsonFile();
    void MakeJson(
        QJsonObject& folder, const QStandardItem* parentFolder);

    QStandardItem* createFolderNode(const QString& folder);
    void createRootNode();
    QStandardItem* createFileNode(const QString& name, qint64 fileId);
    void removeFile();
    bool isRoot(const QStandardItem* item);
    bool isFolder(const QStandardItem* item);

    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void onChange();
private: //Threads
    void tokenizeTexts(const QStringList filePaths, const QString workspace);
    std::pair<Wordlist, Words> tokenizeText(const QString filePath, const QString workspace, std::string fileId);

signals:
    void viewTextClicked(int64_t textId);
    void addTextId(int64_t textId, const std::string& name);
    void removeTextId(int64_t textId);
    void textsTokenized();

public slots:
    //void onAddFolderClicked();
    void onTokenizeText(const QString& filePath);
    void onDeleteFolderClicked();
    void onDeleteFileClicked();
    void onViewTextClicked();

    void onTextTokenized(const QString name, qint64 fileId);
    void onTextsTokenized();
    void onRetranslateUI();
};
