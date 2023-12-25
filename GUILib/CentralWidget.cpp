#include "CentralWidget.h"

#include "Settings.h"
#include "TextTree.h"
#include "BottomMenu.h"

#include <QVBoxLayout>

#include <Wordlists.h>

CentralWidget::CentralWidget(QWidget* parent, Settings* settings)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    mWordlists = new Wordlists(settings->mFile.workspace.toStdString());
    mDlgMgr = new DialogManager(settings, *mWordlists);

    mTextTree = new TextTree(this, settings, mWordlists);
    BottomMenu* bottomMenu = new BottomMenu(this, mTextTree, settings, *mWordlists);
    layout->addWidget(mTextTree);
    layout->addWidget(bottomMenu);

    connect(mTextTree, &TextTree::viewTextClicked, mDlgMgr, &DialogManager::onViewTextClicked);
    connect(mTextTree, &TextTree::addTextId, mDlgMgr, &DialogManager::onAddTextId);
    connect(mTextTree, &TextTree::removeTextId, mDlgMgr, &DialogManager::onRemoveTextId);
    connect(mTextTree, &TextTree::textsTokenized, bottomMenu, &BottomMenu::textsTokenized);
    
    connect(bottomMenu, &BottomMenu::viewWordClicked, mDlgMgr, &DialogManager::onViewWordClicked);
    connect(bottomMenu, &BottomMenu::closeWordViewWindow, mDlgMgr, &DialogManager::onCloseWordViewWindow);

    connect(this, &CentralWidget::shutdownApp, mTextTree, &TextTree::onShutdownApp);
    connect(this, &CentralWidget::retranslateUI, mTextTree, &TextTree::onRetranslateUI);
    connect(this, &CentralWidget::retranslateUI, mDlgMgr, &DialogManager::retranslateUI);
    connect(this, &CentralWidget::retranslateUI, bottomMenu, &BottomMenu::onRetranslateUI);
    connect(this, &CentralWidget::viewWordLists, bottomMenu, &BottomMenu::onViewWordlistsClicked);
}

CentralWidget::~CentralWidget()
{
    delete mWordlists;
}