#include "WordlistsView.h"

#include "Settings.h"
#include "TextTree.h"
#include "DeleteWordlistDialog.h"
#include "EditWordlistDialog.h"
#include "WordView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QStackedWidget>

WordlistsView::WordlistsView(QWidget* parent, TextTree* textTree, Settings* settings, Wordlists& wordlists)
	: QDialog(parent), mTextTree(textTree), mWordlists(wordlists), mSettings(settings)
{
	this->setWindowFlag(Qt::WindowMaximizeButtonHint);
	this->setWindowFlag(Qt::WindowMinimizeButtonHint);
	createLists();
	createConnects();
}

void WordlistsView::createLists()
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	QHBoxLayout* listsLayout = new QHBoxLayout();
	QHBoxLayout* bottomLayout = new QHBoxLayout();
	layout->addLayout(listsLayout);
	layout->addLayout(bottomLayout);

	QVBoxLayout* leftListLayout = new QVBoxLayout();
	QVBoxLayout* middleLayout = new QVBoxLayout();
	QVBoxLayout* rightListLayout = new QVBoxLayout();
	listsLayout->addLayout(leftListLayout);
	listsLayout->addLayout(middleLayout);
	listsLayout->addLayout(rightListLayout);

	mLeftStack = new QStackedWidget();
	mLeftCombobox = new QComboBox();
	mLeftListEditButton = new QPushButton(tr("Edit List Settings"));
	QHBoxLayout* leftComboLayout = new QHBoxLayout();
	leftComboLayout->addWidget(mLeftCombobox);
	leftComboLayout->addWidget(mLeftListEditButton);
	leftListLayout->addLayout(leftComboLayout);
	leftListLayout->addWidget(mLeftStack);

	mMoveRightButton = new QPushButton(QString::fromLocal8Bit("R"));
	mMoveLeftButton = new QPushButton(QString::fromLocal8Bit("L"));
	middleLayout->addWidget(mMoveRightButton);
	middleLayout->addWidget(mMoveLeftButton);

	mRightStack = new QStackedWidget();
	mRightCombobox = new QComboBox();
	mRightListEditButton = new QPushButton(tr("Edit List Settings"));
	QHBoxLayout* rightComboLayout = new QHBoxLayout();
	rightComboLayout->addWidget(mRightCombobox);
	rightComboLayout->addWidget(mRightListEditButton);
	rightListLayout->addLayout(rightComboLayout);
	rightListLayout->addWidget(mRightStack);

	mLeftCombobox->addItem(tr("Unknown Wordlist"), 1);
	setItemColor(mLeftCombobox, 0, mWordlists.getUnknownWordlist().getColor());
	mRightCombobox->addItem(tr("Known Wordlist"), 2);
	setItemColor(mRightCombobox, 0, mWordlists.getKnownWordlist().getColor());
	for (const auto& customWordlistName : mWordlists.getCustomWordlistNames()) {
		WordListInfo::Color color = mWordlists.getCustomWordlist(customWordlistName).getColor();
		mLeftCombobox->addItem(customWordlistName.c_str(), 3);
		setItemColor(mLeftCombobox, mLeftCombobox->count()-1, color);
		mRightCombobox->addItem(customWordlistName.c_str(), 3);
		setItemColor(mLeftCombobox, mRightCombobox->count()-1, color);
	}

	mUnknownList = new QListWidget();
	mUnknownList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mLeftStack->setCurrentIndex(mLeftStack->addWidget(mUnknownList));
	mKnownList = new QListWidget();
	mKnownList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mRightStack->setCurrentIndex(mRightStack->addWidget(mKnownList));

	
	for (const auto& [kana, kanji] : mWordlists.getUnknownWordlist()) {
		QListWidgetItem* item = new QListWidgetItem();
		item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
		item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
		mUnknownList->addItem(item);
	}
	for (const auto& [kana, kanji] : mWordlists.getKnownWordlist()) {
		QListWidgetItem* item = new QListWidgetItem();
		item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
		item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
		mKnownList->addItem(item);
	}

	mAddCustomListButton = new QPushButton(tr("Add Custom List"));
	mDeleteCustomListButton = new QPushButton(tr("Delete Custom List"));
	bottomLayout->addWidget(mAddCustomListButton);
	bottomLayout->addWidget(mDeleteCustomListButton);
}

void WordlistsView::createConnects()
{
	setComboboxConnects();
	connect(mAddCustomListButton, &QPushButton::clicked, this, &WordlistsView::onAddCustomList);
	connect(mDeleteCustomListButton, &QPushButton::clicked, this, &WordlistsView::onDeleteCustomList);
	connect(mMoveRightButton, &QPushButton::clicked, this, &WordlistsView::onMoveRight);
	connect(mMoveLeftButton, &QPushButton::clicked, this, &WordlistsView::onMoveLeft);
	connect(mLeftListEditButton, &QPushButton::clicked, this, &WordlistsView::onEditLeftList);
	connect(mRightListEditButton, &QPushButton::clicked, this, &WordlistsView::onEditRightList);
	connect(mUnknownList, &QListWidget::itemDoubleClicked, this, &WordlistsView::onItemDoubleClicked);
	connect(mKnownList, &QListWidget::itemDoubleClicked, this, &WordlistsView::onItemDoubleClicked);
}

void WordlistsView::setComboboxConnects(bool setConnect)
{
	if (setConnect) {
		connect(mLeftCombobox, &QComboBox::currentIndexChanged, this, &WordlistsView::onLeftChange);
		connect(mRightCombobox, &QComboBox::currentIndexChanged, this, &WordlistsView::onRightChange);
		return;
	}
	disconnect(mLeftCombobox, &QComboBox::currentIndexChanged, this, &WordlistsView::onLeftChange);
	disconnect(mRightCombobox, &QComboBox::currentIndexChanged, this, &WordlistsView::onRightChange);
}

void WordlistsView::onLeftChange(int index)
{
	if (index != -1) {
		onComboboxChange(mLeftCombobox, mRightCombobox, mPrevRightItem, index);
		setCurrentList(mLeftStack, mLeftCombobox);
	}
}

void WordlistsView::onRightChange(int index)
{
	if (index != -1) {
		onComboboxChange(mRightCombobox, mLeftCombobox, mPrevLeftItem, index);
		setCurrentList(mRightStack, mRightCombobox);
	}
}

void WordlistsView::setCurrentList(QStackedWidget* stack, QComboBox* combobox)
{
	switch (combobox->currentData().toInt())
	{
	case 1:
		stack->setCurrentWidget(mUnknownList);
		break;
	case 2:
		stack->setCurrentWidget(mKnownList);
		break;
	case 3:
	{
		std::string currentText = combobox->currentText().toStdString();
		auto iter = mCustomLists.find(currentText);
		if (iter == mCustomLists.end()) {
			mCustomLists[currentText] = new QListWidget();
			mCustomLists[currentText]->setSelectionMode(QAbstractItemView::ExtendedSelection);
			for (const auto& [kana, kanji] : mWordlists.getCustomWordlist(currentText)) {
				QListWidgetItem* item = new QListWidgetItem();
				item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
				item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
				mCustomLists[currentText]->addItem(item);
			}
			connect(mCustomLists[currentText], &QListWidget::itemDoubleClicked, this, &WordlistsView::onItemDoubleClicked);
		}
		if (stack->indexOf(mCustomLists[currentText]) == -1) {
			stack->addWidget(mCustomLists[currentText]);
		}
		stack->setCurrentWidget(mCustomLists[currentText]);
		break;
	}
	default:
		break;
	}
}
// TODO add method to update a QListWidget if already exists when a Wordlist changes
// use QListWidget->clear to delete all items in list
// void WordlistsView::updateList(int wordlistType, const std::string& customListName = "")


void WordlistsView::onComboboxChange(QComboBox* activeCombobox, QComboBox* inactiveCombobox, std::string& previousItem, int index)
{
	setComboboxConnects(false);
	// Index of first custom list name in inactive combobox
	int firstInactiveCustomIndex = inactiveCombobox->findData(3);
	
	if (!previousItem.empty()) {
		int insertIndex = firstInactiveCustomIndex == -1 ? inactiveCombobox->count() : firstInactiveCustomIndex;
		if (firstInactiveCustomIndex != -1) {
			while (insertIndex < inactiveCombobox->count() && inactiveCombobox->itemText(insertIndex) < QString(previousItem.c_str())) {
				insertIndex++;
			}
		}
		// Place the previously selected item in active combobox back into the inactive combobox
		inactiveCombobox->insertItem(insertIndex, previousItem.c_str(), 3);
	}

	if (activeCombobox->currentData() == 3) {
		if (firstInactiveCustomIndex != -1) {
			//Remove selected item from opposite combobox if it is a custom list name
			int removeIndex = firstInactiveCustomIndex;
			while (removeIndex < inactiveCombobox->count() && inactiveCombobox->itemText(removeIndex) != activeCombobox->itemText(index)) {
				removeIndex++;
			}
			inactiveCombobox->removeItem(removeIndex);
		}
		previousItem = activeCombobox->itemText(index).toStdString();
	}
	else {
		previousItem = "";
	}
	setComboboxConnects(true);
}

void WordlistsView::onAddCustomList()
{
	QString customListName = QInputDialog::getText(this, tr("Add Custom List"), tr("Add Custom List"));
	if (customListName.isEmpty()) {
		QMessageBox::information(nullptr, tr("Info"), tr("Custom List already exists!"));
		return;
	}
	if (mWordlists.addCustomWordlist(customListName.toStdString())) {
		mLeftCombobox->addItem(customListName, 3);
		mRightCombobox->addItem(customListName, 3);
	}
}

void WordlistsView::onDeleteCustomList()
{

	DeleteWordlistDialog dialog(mWordlists.getCustomWordlistNames());
	if (dialog.exec()) {
		const auto& [deleteListName, destination] = dialog.result;
		const auto& [destinationType, destinationListName] = destination;

		if (deleteListName.empty()) {
			return;
		}

		if (destinationType != 0) {
			// Move words to different list
			const Wordlist& deleteList = mWordlists.getCustomWordlist(deleteListName);
			switch (destinationType)
			{
			case 1:
				mUnknownList->clear();
				mWordlists.moveWordsToUnknownWordlist(deleteList, deleteListName);
				for (const auto& [kana, kanji] : mWordlists.getUnknownWordlist()) {
					QListWidgetItem* item = new QListWidgetItem();
					item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
					item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
					mUnknownList->addItem(item);
				}
				break;
			case 2:
				mKnownList->clear();
				mWordlists.moveWordsToKnownWordlist(deleteList, deleteListName);
				for (const auto& [kana, kanji] : mWordlists.getKnownWordlist()) {
					QListWidgetItem* item = new QListWidgetItem();
					item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
					item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
					mKnownList->addItem(item);
				}
				break;
			case 3:
				mCustomLists[destinationListName]->clear();
				mWordlists.moveWordsToCustomWordlist(deleteList, destinationListName, deleteListName);
				for (const auto& [kana, kanji] : mWordlists.getCustomWordlist(destinationListName)) {
					QListWidgetItem* item = new QListWidgetItem();
					item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
					item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
					mCustomLists[destinationListName]->addItem(item);
				}
				break;
			default:
				break;
			}
		}
		// Delete custom wordlist file
		mWordlists.removeCustomWordlist(deleteListName);
		// Delete custom wordlist combobox items
		if (mPrevLeftItem != deleteListName) {
			deleteFromCombobox(mLeftCombobox, deleteListName);
		}
		else {
			mPrevLeftItem = "";
		}
		if (mPrevRightItem != deleteListName) {
			deleteFromCombobox(mRightCombobox, deleteListName);
		}
		else {
			mPrevRightItem = "";
		}
		// Delete custom wordlist list widget
		delete mCustomLists[deleteListName];
		mCustomLists.erase(deleteListName);
	}
}

void WordlistsView::deleteFromCombobox(QComboBox* combobox, const std::string& itemText)
{
	// Index of first custom list name in combobox
	int firstCustomIndex = combobox->findData(3);

	if (firstCustomIndex != -1) {
		//Remove selected item from combobox if it is a custom list name
		int removeIndex = firstCustomIndex;
		QString qItemText = itemText.c_str();
		while (removeIndex < combobox->count() && combobox->itemText(removeIndex) != qItemText) {
			removeIndex++;
		}
		// If item to delete is currently selected, select the top item
		if (removeIndex == combobox->currentIndex()) {
			combobox->setCurrentIndex(0);
		}
		setComboboxConnects(false);
		combobox->removeItem(removeIndex);
		setComboboxConnects(true);
	}
}

void WordlistsView::onMoveRight()
{
	onMove(mLeftCombobox, mRightCombobox, mLeftStack, mRightStack);
}

void WordlistsView::onMoveLeft()
{
	onMove(mRightCombobox, mLeftCombobox, mRightStack, mLeftStack);
}

// Move words to different list
void WordlistsView::onMove(QComboBox* sourceCombobox, QComboBox* destCombobox, QStackedWidget* sourceStack, QStackedWidget* destStack)
{
	Wordlist moveList = Wordlist();
	QListWidget* sourceList = dynamic_cast<QListWidget*>(sourceStack->currentWidget());
	QListWidget* destList = dynamic_cast<QListWidget*>(destStack->currentWidget());

	// Remove selected items from source list
	for (QListWidgetItem* item : sourceList->selectedItems()) {
		QStringList qstrings = item->data(Qt::UserRole).toStringList();
		if (qstrings.size() == 2) {
			moveList.insertWord(qstrings[0].toStdString(), qstrings[1].toStdString());
			delete item;
		}
	}

	// Clear destination list, move items to destination wordlist and add all words from destination wordlist to destination list
	destList->clear();
	switch (destCombobox->currentData().toInt())
	{
	case 1:
		mWordlists.moveWordsToUnknownWordlist(moveList);
		for (const auto& [kana, kanji] : mWordlists.getUnknownWordlist()) {
			QListWidgetItem* item = new QListWidgetItem();
			item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
			item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
			destList->addItem(item);
		}
		break;
	case 2:
		mWordlists.moveWordsToKnownWordlist(moveList);
		for (const auto& [kana, kanji] : mWordlists.getKnownWordlist()) {
			QListWidgetItem* item = new QListWidgetItem();
			item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
			item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
			destList->addItem(item);
		}
		break;
	case 3:
		mWordlists.moveWordsToCustomWordlist(moveList, destCombobox->currentText().toStdString());
		for (const auto& [kana, kanji] : mWordlists.getCustomWordlist(destCombobox->currentText().toStdString())) {
			QListWidgetItem* item = new QListWidgetItem();
			item->setText(QString(kana.c_str()) + ", " + kanji.c_str());
			item->setData(Qt::UserRole, { QStringList{QString(kana.c_str()), QString(kanji.c_str())} });
			destList->addItem(item);
		}
		break;
	default:
		break;
	}

	sourceList->clearSelection();
}

void WordlistsView::onEditLeftList()
{
	onEditList(mLeftCombobox);
}

void WordlistsView::onEditRightList()
{
	onEditList(mRightCombobox);
}

void WordlistsView::onEditList(QComboBox* combobox)
{
	EditWordlistDialog dialog(this, combobox->currentData().toInt(), combobox->currentText(), combobox->itemData(combobox->currentIndex(), Qt::UserRole + 1).toInt());
	if (dialog.exec()) {
		WordListInfo::Color color = static_cast<WordListInfo::Color>(dialog.result);
		setItemColor(combobox, combobox->currentIndex(), color);
		WordListInfo::Type type = static_cast<WordListInfo::Type>(combobox->currentData().toInt());
		
		switch (type)
		{
		case WordListInfo::Type::UNKNOWN:
			mWordlists.setUnknownColor(color);
			break;
		case WordListInfo::Type::KNOWN:
			mWordlists.setKnownColor(color);
			break;
		case WordListInfo::Type::CUSTOM:
			mWordlists.setCustomColor(combobox->currentText().toStdString(), color);
			break;
		default:
			break;
		}
	}
}

void WordlistsView::setItemColor(QComboBox *combobox, int index, WordListInfo::Color color)
{
	combobox->setItemData(index, static_cast<int>(color), Qt::UserRole + 1);
	QColor qcolor = QColor(Qt::GlobalColor(WordListInfo::Colors.at(color).second));
	combobox->setItemData(index, qcolor, Qt::DecorationRole);
}

void WordlistsView::onItemDoubleClicked(QListWidgetItem* item)
{
	QStringList qstrings = item->data(Qt::UserRole).toStringList();
	if (qstrings.size() != 2) {
		return;
	}
	emit viewWordClicked(qstrings[0].toStdString(), qstrings[1].toStdString());
}

WordlistsView::~WordlistsView()
{
	;
}