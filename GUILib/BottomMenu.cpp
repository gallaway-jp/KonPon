#include "BottomMenu.h"

#include "TextTree.h"
#include "Settings.h"
#include "Wordlists.h"
#include "WordlistsView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
BottomMenu::BottomMenu(QWidget* parent, TextTree* textTree, Settings* settings, Wordlists& wordlists)
    : QWidget(parent), mTextTree(textTree), _mSettings(settings), mWordlists(wordlists)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QHBoxLayout* bottomLayout2 = new QHBoxLayout();
    layout->addLayout(bottomLayout);
    layout->addLayout(bottomLayout2);
    QPushButton* addFolderButton = new QPushButton(tr("Add Folder"));

    QToolButton* addTextButton = new QToolButton();
    addTextButton->setPopupMode(QToolButton::MenuButtonPopup);
    addTextButton->setText(tr("Add Text"));
    QAction* addAudioAction = addTextButton->addAction(tr("Add Audio"));
    QAction* addTextAudioAction = addTextButton->addAction(tr("Add Text & Audio"));

    QPushButton* deleteFolderButton = new QPushButton(tr("Delete Folder"));
    QPushButton* deleteTextButton = new QPushButton(tr("Delete Text"));
    bottomLayout->addWidget(addFolderButton);
    bottomLayout->addWidget(addTextButton);
    bottomLayout->addWidget(deleteFolderButton);
    bottomLayout->addWidget(deleteTextButton);

    QPushButton* viewTextButton = new QPushButton(tr("View Text"));
    QPushButton* viewWordlistsButton = new QPushButton(tr("View Wordlists"));
    bottomLayout2->addWidget(viewTextButton);
    bottomLayout2->addWidget(viewWordlistsButton);

    connect(addFolderButton, &QPushButton::clicked, this, &BottomMenu::onAddFolderClicked);
    connect(deleteFolderButton, &QPushButton::clicked, this, &BottomMenu::onDeleteFolderClicked);
    connect(addTextButton, &QToolButton::clicked, this, &BottomMenu::onAddTextClicked);
    connect(addAudioAction, &QAction::triggered, this, &BottomMenu::onAddAudioClicked);
    connect(addTextAudioAction, &QAction::triggered, this, &BottomMenu::onAddTextAudioClicked);
    connect(deleteTextButton, &QPushButton::clicked, this, &BottomMenu::onDeleteFileClicked);

    connect(viewTextButton, &QPushButton::clicked, this, &BottomMenu::onViewTextClicked);
    connect(viewWordlistsButton, &QPushButton::clicked, this, &BottomMenu::onViewWordlistsClicked);

    connect(this, &BottomMenu::tokenizeText, mTextTree, &TextTree::onTokenizeText);
    connect(this, &BottomMenu::deleteFolderClicked, mTextTree, &TextTree::onDeleteFolderClicked);
    connect(this, &BottomMenu::deleteFileClicked, mTextTree, &TextTree::onDeleteFileClicked);
    connect(this, &BottomMenu::viewTextClicked, mTextTree, &TextTree::onViewTextClicked);
}

void BottomMenu::onAddFolderClicked()
{
    QString folderName = QInputDialog::getText(this, tr("Add Folder"), tr("Add Folder2"));
    if (!folderName.isEmpty()) {
        mTextTree->addFolder(folderName);
    }
}

void BottomMenu::onAddTextClicked()
{
    emit tokenizeText(_mSettings->mFile.workspace);
}

void BottomMenu::onAddAudioClicked()
{
    int a = 0;
}

void BottomMenu::onAddTextAudioClicked()
{
    int a = 0;
}

void BottomMenu::onDeleteFolderClicked()
{
    emit deleteFolderClicked();
}

void BottomMenu::onDeleteFileClicked()
{
    emit deleteFileClicked();
}

void BottomMenu::onViewTextClicked()
{
    emit viewTextClicked();
}

void BottomMenu::onViewWordlistsClicked()
{
    if (!mWordlistsView) {
        mWordlistsView = new WordlistsView(this, mTextTree, _mSettings, mWordlists);
        connect(mWordlistsView, &WordlistsView::viewWordClicked, this, &BottomMenu::viewWordClicked);
    }
    mWordlistsView->show();
}