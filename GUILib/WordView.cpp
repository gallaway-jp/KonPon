#include "WordView.h"

#include "Settings.h"
#include "Words.h"
#include "Wordlists.h"
#include "TextView.h"
#include "CreateAnkiCardDialog.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QClipboard>
#include <QApplication>
#include <QTooltip>

#include <map>

WordView::WordView(const std::string& kana, const std::string& kanji, Settings* settings, Wordlists& wordlists, TextIdsMap& textIds)
	: QDialog(nullptr), mSettings(settings), mWordlists(wordlists), mTextIds(textIds),
	mTextWord(kana, kanji, settings->mFile.workspace.toStdString())
{
	setDialogProperties();

	constexpr int layoutSpacing = 50;
	layout = new QFormLayout(this);
	layout->setHorizontalSpacing(layoutSpacing);

	addKanaKanjiLineWidgets(layout);

	addPitchAccentsLineWidget(layout);

	addTextListWidget(layout);

	layout->addRow(tr("Notes"), (QWidget*)nullptr);
	mNotesRow = layout->rowCount() - 1;

	mNotesEdit = new QTextEdit("TODO replace this with notes data from word info");
	layout->addRow(mNotesEdit);

	mShowMoreButton = new QPushButton();
	mShowMoreButton->setCheckable(true);
	connect(mShowMoreButton, &QPushButton::toggled, this, &WordView::onToggledShowMore);
	layout->addRow(mShowMoreButton);
	onToggledShowMore(false);

	mCopyButton = new QPushButton(tr("Copy Word Info"));
	connect(mCopyButton, &QPushButton::clicked, this, &WordView::onCopyWordInfo);
	layout->addRow(mCopyButton);

	// TODO implement Create Anki Card behavior
	if (mSettings->mAnki.isAnkiConnectFeatureEnabled) {
		QPushButton* createAnkiCardButton = new QPushButton(tr("Create Anki Card"));
		connect(createAnkiCardButton, &QPushButton::clicked, this, &WordView::onCreateAnkiCardButton);
		layout->addRow(createAnkiCardButton);
	}
}

void WordView::setDialogProperties()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlag(Qt::WindowMaximizeButtonHint);
	setWindowFlag(Qt::WindowMinimizeButtonHint);
	setWindowTitle(tr("Word View"));
}

void WordView::addKanaKanjiLineWidgets(QFormLayout* layout)
{
	QLineEdit* kanaLineEdit = new QLineEdit(mTextWord.getKana().c_str());
	kanaLineEdit->setReadOnly(true);
	layout->addRow(tr("Kana: "), kanaLineEdit);

	QLineEdit* kanjiLineEdit = new QLineEdit(mTextWord.getKanji().c_str());
	kanjiLineEdit->setReadOnly(true);
	layout->addRow(tr("Kanji: "), kanjiLineEdit);
}

void WordView::addPitchAccentsLineWidget(QFormLayout* layout)
{
	QString pitchAccentsString;
	for (auto pitchAccent : mTextWord.getPitchAccents()) {
		bool prependComma = !pitchAccentsString.isEmpty();
		pitchAccentsString += (prependComma ? ", " : "") + QString::number(pitchAccent);
	}

	QLineEdit* accentsLineEdit = new QLineEdit(pitchAccentsString);
	accentsLineEdit->setReadOnly(true);
	layout->addRow(tr("Pitch Accents"), accentsLineEdit);
}

void WordView::addTextListWidget(QFormLayout* layout)
{
	mTextList = new QListWidget();
	for (auto textID : mTextWord.getTextIds()) {
		if (auto it = mTextIds.find(textID); it != mTextIds.end()) {
			QListWidgetItem* item = new QListWidgetItem(it->second.first.c_str());
			item->setData(Qt::UserRole, textID);
			mTextList->addItem(item);
		}
		//TODO text in word info should really be deleted on iteration of words in text's wordlist
	}

	layout->addRow(tr("Texts"), (QWidget*)nullptr);
	layout->addRow(mTextList);

	connect(mTextList, &QListWidget::itemClicked, this, &WordView::onTextClicked);
}

void WordView::onTextClicked(QListWidgetItem* item)
{
	// Open the textview for the selected file
	int64_t textId = item->data(Qt::UserRole).toLongLong();
	emit viewTextClicked(textId);
}

void WordView::onRemoveTextIdFromWordInfo(int64_t textId)
{
	if (auto it = mTextListItems.find(textId); it != mTextListItems.end()) {
		delete mTextList->takeItem(mTextList->row(it->second));
		mTextWord.removeTextId(textId);
		mTextListItems.erase(it);
	}
}

void WordView::onCreateAnkiCardButton()
{
	CreateAnkiCardDialog dialog(this, mTextWord.getKana(), mTextWord.getKanji());
	dialog.exec();
}

void WordView::onCopyWordInfo()
{
	QString text;
	text += "Kana: ";
	text += mTextWord.getKana().c_str();
	text += "\nKanji: ";
	text += mTextWord.getKanji().c_str();

	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(text, QClipboard::Clipboard);

	if (clipboard->supportsSelection()) {
		clipboard->setText(text, QClipboard::Selection);
	}

#if defined(Q_OS_LINUX)
#include "QThread"
	QThread::msleep(1); //workaround for copied text not being available...
#endif
	constexpr int second = 1000; //1000 milliseconds in 1 second
	constexpr int tooltipDuration = 3 * second; // tooltip is shown for 3 seconds
	//setToolTip(tr("Word info copied to clipboard."));
	QToolTip::showText(mCopyButton->mapToGlobal(mCopyButton->rect().topLeft()), tr("Word info copied to clipboard."),
		nullptr, {}, tooltipDuration);
}

void WordView::onToggledShowMore(bool checked)
{
	if (checked) {
		if (!mNotesEdit->isVisible()) {
			mShowMoreButton->setText(tr("Show Less"));
			layout->setRowVisible(mNotesRow, true);
			layout->setRowVisible(mNotesEdit, true);
		}
		return;
	}
	mShowMoreButton->setText(tr("Show More"));
	layout->setRowVisible(mNotesRow, false);
	layout->setRowVisible(mNotesEdit, false);
}

WordView::~WordView()
{
	emit closeDialog();
}