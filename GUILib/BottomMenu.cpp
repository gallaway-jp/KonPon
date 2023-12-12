#include "BottomMenu.h"

#include "TextTree.h"
#include "Settings.h"
#include "Wordlists.h"
#include "WordlistsView.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

BottomMenu::BottomMenu(QWidget* parent, TextTree* textTree, Settings* settings, Wordlists& wordlists)
    : QWidget(parent), mTextTree(textTree), _mSettings(settings), mWordlists(wordlists)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QHBoxLayout* bottomLayout2 = new QHBoxLayout();
    layout->addLayout(bottomLayout);
    layout->addLayout(bottomLayout2);
    m_addFolderButton = new QPushButton(tr("Add Folder"));

    m_addTextButton = new QToolButton();
    m_addTextButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_addTextButton->setText(tr("Add Text"));
    m_addAudioAction = m_addTextButton->addAction(tr("Add Audio"));
    m_addAudioAction->setEnabled(false);
    m_addTextAudioAction = m_addTextButton->addAction(tr("Add Text && Audio"));
    m_addTextAudioAction->setEnabled(false);

    m_deleteFolderButton = new QPushButton(tr("Delete Folder"));
    m_deleteTextButton = new QPushButton(tr("Delete Text"));
    bottomLayout->addWidget(m_addFolderButton);
    bottomLayout->addWidget(m_addTextButton);
    bottomLayout->addWidget(m_deleteFolderButton);
    bottomLayout->addWidget(m_deleteTextButton);

    m_viewTextButton = new QPushButton(tr("View Text"));
    m_viewWordlistsButton = new QPushButton(tr("View Wordlists"));
    bottomLayout2->addWidget(m_viewTextButton);
    bottomLayout2->addWidget(m_viewWordlistsButton);

    connect(m_addFolderButton, &QPushButton::clicked, this, &BottomMenu::onAddFolderClicked);
    connect(m_deleteFolderButton, &QPushButton::clicked, this, &BottomMenu::onDeleteFolderClicked);
    connect(m_addTextButton, &QToolButton::clicked, this, &BottomMenu::onAddTextClicked);
    connect(m_addAudioAction, &QAction::triggered, this, &BottomMenu::onAddAudioClicked);
    connect(m_addTextAudioAction, &QAction::triggered, this, &BottomMenu::onAddTextAudioClicked);
    connect(m_deleteTextButton, &QPushButton::clicked, this, &BottomMenu::onDeleteFileClicked);

    connect(m_viewTextButton, &QPushButton::clicked, this, &BottomMenu::onViewTextClicked);
    connect(m_viewWordlistsButton, &QPushButton::clicked, this, &BottomMenu::onViewWordlistsClicked);

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
        connect(mWordlistsView, &WordlistsView::closeWordViewWindow, this, &BottomMenu::closeWordViewWindow);
        connect(this, &BottomMenu::textsTokenized, mWordlistsView, &WordlistsView::onReloadWordlists);
        connect(this, &BottomMenu::retranslateUI, mWordlistsView, &WordlistsView::onRetranslateUI);
    }
    mWordlistsView->show();
    mWordlistsView->raise();
    mWordlistsView->activateWindow();
}

void BottomMenu::onRetranslateUI()
{
    m_addFolderButton->setText(QCoreApplication::translate("BottomMenu", "Add Folder"));
    m_addTextButton->setText(QCoreApplication::translate("BottomMenu", "Add Text"));
    m_addAudioAction->setText(QCoreApplication::translate("BottomMenu", "Add Audio"));
    m_addTextAudioAction->setText(QCoreApplication::translate("BottomMenu", "Add Text && Audio"));
    m_deleteFolderButton->setText(QCoreApplication::translate("BottomMenu", "Delete Folder"));
    m_deleteTextButton->setText(QCoreApplication::translate("BottomMenu", "Delete Text"));
    m_deleteTextButton->setText(QCoreApplication::translate("BottomMenu", "Delete Text"));
    m_viewTextButton->setText(QCoreApplication::translate("BottomMenu", "View Text"));
    m_viewWordlistsButton->setText(QCoreApplication::translate("BottomMenu", "View Wordlists"));

    emit retranslateUI();
}

BottomMenu::~BottomMenu()
{
    delete mWordlistsView;
}