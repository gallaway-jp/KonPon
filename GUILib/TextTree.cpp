#include "TextTree.h"

#include "TextView.h"
#include "Settings.h"
#include "Wordlists.h"
#include "DialogManager.h"

#include <chrono>

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QModelIndex>
#include <QFileIconProvider>

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include <QByteArray>

TextTree::TextTree(QWidget* parent, Settings* settings, Wordlists* wordlists)
    : _mSettings(settings), QTreeView(parent), mWordlists(*wordlists)
{
    _mModel = new QStandardItemModel;
    QTreeView::setModel(_mModel);

    createRootNode();

    TextTreeReadFromJsonFile();

    QTreeView::setDragDropMode(QAbstractItemView::InternalMove);
    QTreeView::setHeaderHidden(true);

    connect(this, &TextTree::textsTokenized, this, &TextTree::onTextsTokenized);
}

void TextTree::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeView::dragEnterEvent(event);
}

bool TextTree::isRoot(const QStandardItem* item)
{
    return item->data().toLongLong() == -1;
}

bool TextTree::isFolder(const QStandardItem* item)
{
    return item->data().toLongLong() == 0;
}

void TextTree::dropEvent(QDropEvent* event)
{
    QModelIndex mi = QTreeView::indexAt(event->pos());
    QStandardItem* item = _mModel->itemFromIndex(mi);

    if (item && isFolder(item)) {
        //If drop destination is a folder, add item to the folder
        QTreeView::dropEvent(event);
        onChange();
        return;
    }

    QStandardItem* dropSource = _mModel->itemFromIndex(QTreeView::selectionModel()->currentIndex());
    if (!dropSource) {
        return;
    }
    
    // Remove source item from its parent
    QStandardItem* sourceParent = dropSource->parent();
    if (!sourceParent) {
        // Treat root as parent
        sourceParent = _mModel->invisibleRootItem();
    }
    for (QStandardItem* dropSourceItem : sourceParent->takeRow(dropSource->row())) {
        dropSource = dropSourceItem;
        // There should only be one item since there is only one column
        break;
    }

    if (!item) {
        // If drop destination is not an item, add to root
        _mModel->appendRow(dropSource);
        onChange();
        return;
    }

    // If drop destination is a file, then add item to parent folder
    QStandardItem* parent = item->parent();
    if (!parent) {
        // Treat root as parent
        parent = _mModel->invisibleRootItem();
    }
    parent->appendRow(dropSource);
    onChange();
}

void TextTree::parseElement(const QString& key, const QJsonValue& value, QStandardItem* currentFolder)
{
    // Empty Folder
    if (value.isNull()) {
        QStandardItem* emptyFolder = createFolderNode(key);
        currentFolder->appendRow(emptyFolder);
    }

    // Subfolder
    else if (value.isObject()) {
        ParseFolder(key, value.toObject(), currentFolder);
    }

    // File
    else if (value.isDouble()) {
        qint64 fileId = value.toInteger();
        QStandardItem* file = createFileNode(key, fileId);
        currentFolder->appendRow(file);
    }
}

void TextTree::ParseFolder(const QString& folderName,
    const QJsonObject& folder, QStandardItem *parentFolder)
{
    QJsonObject::iterator iter;
    QJsonObject folderObject = folder;

    QStandardItem* currentFolder = _mModel->invisibleRootItem();
    if(!folderName.isEmpty()) {
        currentFolder = createFolderNode(folderName);
        parentFolder->appendRow(currentFolder);
    }

    for (iter = folderObject.begin();
        iter != folderObject.end(); ++iter)
    {
        if (iter.value().isArray()) {
            for (const QJsonValue& element : iter.value().toArray()) {
                parseElement(iter.key(), element, currentFolder);
            }
            continue;
        }
        parseElement(iter.key(), iter.value(), currentFolder);
    }
}

bool TextTree::TextTreeReadFromJsonFile()
{
    QFile file(QDir(_mSettings->mFile.workspace + QString("/KonPonData")).absoluteFilePath("TextTree.json"));
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonObject rootObj = doc.object();
    ParseFolder("", rootObj, _mModel->invisibleRootItem());
    return true;
}

void TextTree::MakeJson(
    QJsonObject& folderObject, const QStandardItem* parentFolder)
{
    int rowCount = parentFolder->rowCount();
    for (int i = 0; i < rowCount; i++) {
        QStandardItem* item = parentFolder->child(i);
        if (!item) {
            continue;
        }
        if (isRoot(item)) {
            // Root is always the same, so no need to save
            continue;
        }

        qint64 fileId = item->data().toLongLong();
        QString name = item->text();
        if (folderObject.contains(name)) {
            const QJsonValue& value = folderObject[name];
            QJsonArray jsonArray;
            if (value.isArray()) {
                jsonArray = value.toArray();
            }
            else {
                // If non-array value, append existing value to new array
                jsonArray.append(value);
            }
            if (fileId == 0) {
                if (item->rowCount() == 0) {
                    // Add empty folder
                    jsonArray.append(QJsonValue());
                }
                else {
                    // Add non-empty folder
                    QJsonObject subfolderObject;
                    MakeJson(subfolderObject, item);
                    jsonArray.append(subfolderObject);
                }
            }
            else {
                // Add file
                jsonArray.append(QJsonValue(fileId));
            }
            folderObject[name] = jsonArray;
        }
        else if (fileId == 0) {
            if (item->rowCount() == 0) {
                //Empty Folder
                folderObject[name] = QJsonValue();
            }
            else {
                QJsonObject subfolderObject;
                MakeJson(subfolderObject, item);
                folderObject[name] = subfolderObject;
            }
        }
        else {
            //File
            folderObject[name] = QJsonValue(fileId);
        }
    }
}

bool TextTree::TextTreeWriteToJsonFile()
{
    QFile file(QDir(QCoreApplication::applicationDirPath() + QString("/KonPonData")).absoluteFilePath("TextTree.json"));
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject object;
    MakeJson(object, _mModel->invisibleRootItem());

#ifdef QT_DEBUG
    QJsonDocument::JsonFormat format = QJsonDocument::Indented;
#else
    QJsonDocument::JsonFormat format = QJsonDocument::Compact;
#endif

    bool writeSucceeded = file.write(QJsonDocument(object).toJson(format)) > 0;
    file.close();
    return writeSucceeded;
}

void TextTree::addFolder(const QString& folder)
{
    if (!_mIsSaveNeeded) {
        onChange();
    }

    QModelIndex mi = QTreeView::selectionModel()->currentIndex();
    QStandardItem* item = _mModel->itemFromIndex(mi);

    QStandardItem* emptyFolder = createFolderNode(folder);
    if (!item || isRoot(item)) {
        // Add to root of tree
        _mModel->appendRow(emptyFolder);
        return;
    }

    if (isFolder(item)) {
        //Current item is a folder, so add folder in this folder;
        item->appendRow(emptyFolder);
        return;
    }

    //Current item is a file, so add folder to parent folder;
    QStandardItem* parent = item->parent();
    if (!parent) {
        // Treat top as parent
        parent = _mModel->invisibleRootItem();
    }
    parent->appendRow(emptyFolder);
}

void TextTree::removeFolder()
{
    if (!_mIsSaveNeeded) {
        onChange();
    }

    QModelIndex mi = QTreeView::selectionModel()->currentIndex();
    QStandardItem* item = _mModel->itemFromIndex(mi);

    if (!item || !isFolder(item)) {
        // No item available, item is root, or item is not folder, so do nothing
        return;
    }

    QStandardItem* parent = item->parent();
    if (!parent) {
        // Treat root as parent
        parent = _mModel->invisibleRootItem();
    }

    //Current item is a folder, so move its child items to its parent folder;
    if (item->hasChildren()) {
        for (int i = 0; i < item->rowCount(); i++) {
            for (QStandardItem* child : item->takeRow(i)){
                if (child) {
                    parent->appendRow(child);
                }
            }
        }
    }
    // Remove folder
    parent->removeRow(item->row());
}

QStandardItem* TextTree::createFolderNode(const QString& name)
{
    QFileIconProvider provider;
    QIcon folderIcon = provider.icon(QFileIconProvider::Folder);

    QStandardItem* folder = new QStandardItem(folderIcon, name);
    folder->setData(QVariant(0));
    return folder;
}

void TextTree::createRootNode()
{
    _mRoot = new QStandardItem(tr("Root"));
    _mRoot->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
    _mRoot->setData(QVariant(-1));
    _mModel->appendRow(_mRoot);
}

void TextTree::removeFile()
{
    if (!_mIsSaveNeeded) {
        onChange();
    }

    QModelIndex mi = QTreeView::selectionModel()->currentIndex();
    QStandardItem* item = _mModel->itemFromIndex(mi);

    if (!item || isFolder(item) || isRoot(item)) {
        // No item available, item is root, or item is folder, so do nothing
        return;
    }

    QStandardItem* parent = item->parent();
    if (!parent) {
        // Treat root as parent
        parent = _mModel->invisibleRootItem();
    }

    qint64 fileId = item->data().toLongLong();

    // Remove file
    parent->removeRow(item->row());

    QDir dir = QDir(_mSettings->mFile.workspace + QString("/KonPonData") + QString("/Texts") + QString("/") + QString::number(fileId));
    dir.removeRecursively();

    Words wordsInfo;
    wordsInfo.setWorkspace(_mSettings->mFile.workspace.toStdString());
    wordsInfo.removeTextId(fileId);

    emit removeTextId(fileId);
}

QStandardItem* TextTree::createFileNode(const QString& name, qint64 fileId)
{
    QFileIconProvider provider;
    QIcon fileIcon = provider.icon(QFileIconProvider::File);
    QStandardItem* file = new QStandardItem(fileIcon, name);
    file->setFlags(file->flags() | Qt::ItemNeverHasChildren);
    file->setData(QVariant(fileId));
    return file;
}

TextTree::~TextTree()
{
    if (_mIsSaveNeeded) {
        TextTreeWriteToJsonFile();
    }
}

void TextTree::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return){
        onViewTextClicked();
        event->accept();
        return;
    }
    QTreeView::keyPressEvent(event); // call the default implementation
}

void TextTree::mouseDoubleClickEvent(QMouseEvent* event)
{
    onViewTextClicked();
    event->accept();
}

void TextTree::mousePressEvent(QMouseEvent* event)
{
    QModelIndex mi = QTreeView::selectionModel()->currentIndex();
    QStandardItem* item = _mModel->itemFromIndex(mi);
    if (item) {
        mMousePressedTimer.start();
    }
    QTreeView::mousePressEvent(event);
}

void TextTree::mouseReleaseEvent(QMouseEvent* event)
{
    QModelIndex mi = QTreeView::selectionModel()->currentIndex();
    QStandardItem* item = _mModel->itemFromIndex(mi);
    if (item && mMousePressedTimer.isValid() && mMousePressedTimer.elapsed() >= 500) {
        mMousePressedTimer.invalidate();
        TextTree::edit(mi);
        event->accept();
        return;
    }
    mMousePressedTimer.invalidate();
    QTreeView::mouseReleaseEvent(event);
}

void TextTree::onChange()
{
    _mIsSaveNeeded = true;
}

#include "TextParser.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QByteArray>
#include <QtConcurrent>
void TextTree::tokenizeTexts(const QStringList filePaths, const QString workspace)
{
    std::vector<QFuture<std::pair<Wordlist, Words>>> promises;
    for (QString filePath : filePaths) {
        std::string fileId = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        QFuture<std::pair<Wordlist, Words>> promise = QtConcurrent::run([this, filePath, workspace, fileId]() { return tokenizeText(filePath, workspace, fileId); });
        promises.push_back(promise);
    }
    Words mergedWords;
    for (auto promise : promises) {
        promise.waitForFinished();
        Wordlist wordlist = promise.result().first;
        // Add words to unknown wordlist
        mWordlists.addWordsToUnknownWordlist(wordlist);
        for (auto [key, val] : promise.result().second) {
            mergedWords.insertWord(val);
        }
    }
    //// Update individual word info files
    mergedWords.setWorkspace(workspace.toStdString());


    emit textsTokenized();
}

std::pair<Wordlist, Words> TextTree::tokenizeText(const QString filePath, const QString workspace, const std::string fileId)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(nullptr, tr("Error"), file.errorString());
        return {};
    }
    
    QByteArray ba;
    {
        QTextStream ReadFile(&file);
        QString completeText;
        QString currentLine;
        bool isFirstLine = true;
        while (ReadFile.readLineInto(&currentLine) != false) {
            if (!isFirstLine) {
                completeText.append("\n");
            }
            completeText.append(currentLine);
            isFirstLine = false;
        }
        ba = completeText.toUtf8();
    }

    Wordlist wordlist;
    Words newWords;
    qint64 result = Parser::TokenizeText(workspace.toStdString(), filePath.toStdString(), ba.data(),
        fileId, wordlist, newWords);
    if (result != 0) {
        emit onTextTokenized(QFileInfo(file.fileName()).baseName(), result);
    }
    return { wordlist, newWords };
}

void TextTree::onTokenizeText(const QString& workspace)
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Open Text"), "", tr("Text Files (*.txt)"));
    QFuture<void> promise = QtConcurrent::run([this, filePaths, workspace](){ tokenizeTexts(filePaths, QString(workspace)); });
}

void TextTree::onTextTokenized(const QString name, qint64 fileId)
{
    if (!_mIsSaveNeeded) {
        onChange();
    }

    //Add file to tree
    QStandardItem* file = createFileNode(name, fileId);
    _mModel->appendRow(file);

    //Add id,name to TextIds.json
    emit addTextId(fileId, name.toStdString());
    
}

void TextTree::onTextsTokenized()
{
    if (!_mIsSaveNeeded) {
        onChange();
    }

    //TODO enable buttons again
    
}

void TextTree::onDeleteFolderClicked()
{
    removeFolder();
}

void TextTree::onDeleteFileClicked()
{
    removeFile();
}

void TextTree::onViewTextClicked()
{
    QModelIndex mi = QTreeView::selectionModel()->currentIndex();
    QStandardItem* item = _mModel->itemFromIndex(mi);
    if (!item || isFolder(item) || isRoot(item)) {
        return;
    }
    int64_t textId = item->data().toLongLong();
    
    emit viewTextClicked(textId);
}

void TextTree::onRetranslateUI()
{
    _mRoot->setText(QCoreApplication::translate("TextTree", "Root"));
}