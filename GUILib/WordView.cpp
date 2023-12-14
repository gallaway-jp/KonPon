#include "WordView.h"

#include "Settings.h"
#include "Words.h"
#include "Wordlists.h"
#include "TextView.h"
#include "CreateAnkiCardDialog.h"

#include <QApplication>
#include <QClipboard>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QTooltip>

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

	m_notesLabel = new QLabel(tr("Notes"));
	layout->addRow(m_notesLabel, (QWidget*)nullptr);
	mNotesRow = layout->rowCount() - 1;

	mNotesEdit = new QTextEdit();
	mNotesEdit->setPlainText(mTextWord.getNotes().c_str());
	layout->addRow(mNotesEdit);
	connect(mNotesEdit, &QTextEdit::textChanged, this, &WordView::onNotesTextChanged);

	mShowMoreButton = new QPushButton();
	mShowMoreButton->setCheckable(true);
	connect(mShowMoreButton, &QPushButton::toggled, this, &WordView::onToggledShowMore);
	layout->addRow(mShowMoreButton);
	onToggledShowMore(false);

	mCopyButton = new QPushButton(tr("Copy Word Info"));
	connect(mCopyButton, &QPushButton::clicked, this, &WordView::onCopyWordInfo);
	layout->addRow(mCopyButton);

	if (mSettings->mAnki.isAnkiConnectFeatureEnabled) {
		m_createAnkiCardButton = new QPushButton(tr("Create Anki Card"));
		connect(m_createAnkiCardButton, &QPushButton::clicked, this, &WordView::onCreateAnkiCardButton);
		layout->addRow(m_createAnkiCardButton);
	}
}

void WordView::setDialogProperties()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlag(Qt::WindowMaximizeButtonHint);
	setWindowFlag(Qt::WindowMinimizeButtonHint);
	setWindowTitle(tr("Word Info"));
}

void WordView::addKanaKanjiLineWidgets(QFormLayout* layout)
{
	QLineEdit* kanaLineEdit = new QLineEdit(mTextWord.getKana().c_str());
	kanaLineEdit->setReadOnly(true);
	m_kanaLabel = new QLabel(tr("Kana: "));
	layout->addRow(m_kanaLabel, kanaLineEdit);

	QLineEdit* kanjiLineEdit = new QLineEdit(mTextWord.getKanji().c_str());
	kanjiLineEdit->setReadOnly(true);
	m_kanjiLabel = new QLabel(tr("Kanji: "));
	layout->addRow(m_kanjiLabel, kanjiLineEdit);
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
	m_pitchAccentsLabel = new QLabel(tr("Pitch Accents"));
	layout->addRow(m_pitchAccentsLabel, accentsLineEdit);
}

void WordView::addTextListWidget(QFormLayout* layout)
{
	mTextList = new QListWidget();
	for (auto textID : mTextWord.getTextIds()) {
		if (auto it = mTextIds.find(textID); it != mTextIds.end()) {
			QListWidgetItem* item = new QListWidgetItem(it->second.first.c_str());
			item->setData(Qt::UserRole, textID);
			mTextList->addItem(item);
			mTextListItems[textID] = item;
		}
	}

	m_textsLabel = new QLabel(tr("Texts"));
	layout->addRow(m_textsLabel, (QWidget*)nullptr);
	layout->addRow(mTextList);

	connect(mTextList, &QListWidget::itemClicked, this, &WordView::onTextClicked);
}

void WordView::onTextClicked(QListWidgetItem* item)
{
	// Open the textview for the selected file
	int64_t textId = item->data(Qt::UserRole).toLongLong();
	emit viewTextClicked(textId);
}

void WordView::onRemoveTextId(int64_t textId)
{
	if (auto it = mTextListItems.find(textId); it != mTextListItems.end()) {
		delete mTextList->takeItem(mTextList->row(it->second));
		mTextListItems.erase(it);
	}
}

void WordView::onRetranslateUI()
{
	setWindowTitle(QCoreApplication::translate("WordView", "Word Info"));

	m_kanaLabel->setText(QCoreApplication::translate("WordView", "Kana: "));
	m_kanjiLabel->setText(QCoreApplication::translate("WordView", "Kanji: "));
	m_pitchAccentsLabel->setText(QCoreApplication::translate("WordView", "Pitch Accents"));
	m_textsLabel->setText(QCoreApplication::translate("WordView", "Texts"));
	m_notesLabel->setText(QCoreApplication::translate("WordView", "Notes"));

	if (layout->isRowVisible(mNotesRow)) {
		mShowMoreButton->setText(QCoreApplication::translate("WordView", "Show Less"));
	}
	else {
		mShowMoreButton->setText(QCoreApplication::translate("WordView", "Show More"));
	}

	mCopyButton->setText(QCoreApplication::translate("WordView", "Copy Word Info"));
	m_createAnkiCardButton->setText(QCoreApplication::translate("WordView", "Create Anki Card"));
}

void WordView::onCreateAnkiCardButton()
{
	CreateAnkiCardDialog dialog(this, mTextWord.getKana(), mTextWord.getKanji(), mSettings);
	dialog.exec();
}

void WordView::onCopyWordInfo()
{
	QString text;
	text += "Kana: ";
	text += mTextWord.getKana().c_str();
	text += "\nKanji: ";
	text += mTextWord.getKanji().c_str();
	text += "\nPitch Accents: ";
	bool first = true;
	for (auto pitchAccent : mTextWord.getPitchAccents())
	{
		if (!first) {
			text += ", ";
		}
		first = false;
		text += std::to_string(pitchAccent).c_str();
	}

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

void WordView::onNotesTextChanged()
{
	mRequiresNotesUpdate = true;
}

WordView::~WordView()
{
	if (mRequiresNotesUpdate) {
		mTextWord.setNotes(mNotesEdit->toPlainText().toStdString());
	}
	emit closeDialog();
}