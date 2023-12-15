#include "AnkiCardEditor.h"
#include "DragWidget.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QScrollArea>
#include <QTextCursor>
#include <QPainter>
#include <QColor>
#include <QColorDialog>
#include "ListDialog.h"
#include <QCoreApplication>
AnkiCardEditor::AnkiCardEditor()
	: QWidget(nullptr)
{
	setTextWidth();

	mLayout = new QVBoxLayout(this);

	createNoteDeckButtons();
	
	QVBoxLayout* toolbarLayout = new QVBoxLayout();
	toolbarLayout->addWidget(createEditToolbar());
	QFrame* toolbarFrame = new QFrame();
	toolbarLayout->setContentsMargins(0, 0, 0, 0);
	toolbarFrame->setFrameShape(QFrame::StyledPanel);
	toolbarFrame->setLayout(toolbarLayout);
	mLayout->addWidget(toolbarFrame);
	
	createFields();

	mTagEdit = new QLineEdit();
	QFormLayout* tagsLayout = new QFormLayout();
	mLayout->addLayout(tagsLayout);
	tagsLayout->addRow(QCoreApplication::translate("AnkiCardEditor", "Tags"), mTagEdit);
}

void AnkiCardEditor::setTextWidth()
{
	mTextWidth = 50;
}

void AnkiCardEditor::createNoteDeckButtons()
{
	QHBoxLayout* deckNoteLayout = new QHBoxLayout();
	mLayout->addLayout(deckNoteLayout);

	QFormLayout* noteTypeLayout = new QFormLayout();
	deckNoteLayout->addLayout(noteTypeLayout);
	mNoteTypeButton = new QPushButton(mCurrentNoteType);
	noteTypeLayout->addRow(QCoreApplication::translate("AnkiCardEditor", "Note Type"), mNoteTypeButton);
	QFormLayout* deckLayout = new QFormLayout();
	deckNoteLayout->addLayout(deckLayout);
	mDeckButton = new QPushButton(mCurrentDeck);
	deckLayout->addRow(QCoreApplication::translate("AnkiCardEditor", "Deck Type"), mDeckButton);

	connect(mNoteTypeButton, &QPushButton::clicked, this, &AnkiCardEditor::onNoteTypeButtonClicked);
	connect(mDeckButton, &QPushButton::clicked, this, &AnkiCardEditor::onDeckButtonClicked);
}

void AnkiCardEditor::createFields()
{
	QWidget* widget = new QWidget();
	widget->setContentsMargins(0, 0, 0, 0);
	scrollArea = new QScrollArea();
	mLayout->addWidget(scrollArea);

	fieldsLayout = new QFormLayout();
	fieldsLayout->setContentsMargins(0, 0, 0, 0);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setWidgetResizable(true);
	
	widget->setLayout(fieldsLayout);
	scrollArea->setWidget(widget);

	setFields(mCurrentNoteType);

	scrollArea->setMaximumHeight((fieldHeight + fieldsLayout->verticalSpacing()) * 2);
}

QToolBar* AnkiCardEditor::createEditToolbar()
{
	QToolBar* cardEditMenuBar = new QToolBar();
	
	m_boldAction = cardEditMenuBar->addAction("B", QKeySequence::Bold);
	m_boldAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Bold"));
	m_boldAction->setCheckable(true);
	m_boldAction->setChecked(false);
	QFont font = QFont(m_boldAction->font());
	font.setBold(true);
	m_boldAction->setFont(font);
	connect(m_boldAction, &QAction::toggled, this, &AnkiCardEditor::onBoldActionToggled);
	m_boldAction->setChecked(false);

	m_italicAction = cardEditMenuBar->addAction("I", QKeySequence::Italic);
	m_italicAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Italic"));
	m_italicAction->setCheckable(true);
	m_italicAction->setChecked(false);
	font.setBold(false);
	font.setItalic(true);
	m_italicAction->setFont(font);
	connect(m_italicAction, &QAction::toggled, this, &AnkiCardEditor::onItalicActionToggled);

	m_underlineAction = cardEditMenuBar->addAction("U", QKeySequence::Underline);
	m_underlineAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Underline"));
	m_underlineAction->setCheckable(true);
	m_underlineAction->setChecked(false);
	font.setItalic(false);
	font.setUnderline(true);
	m_underlineAction->setFont(font);
	connect(m_underlineAction, &QAction::toggled, this, &AnkiCardEditor::onUnderlineActionToggled);

	m_strikeoutAction = cardEditMenuBar->addAction("S", Qt::Key_F7);
	m_strikeoutAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Strikeout"));
	m_strikeoutAction->setCheckable(true);
	m_strikeoutAction->setChecked(false);
	font.setUnderline(false);
	font.setStrikeOut(true);
	m_strikeoutAction->setFont(font);
	connect(m_strikeoutAction, &QAction::toggled, this, &AnkiCardEditor::onStrikeoutActionToggled);

	cardEditMenuBar->addSeparator();

	font.setStrikeOut(false);
	cardEditMenuBar->setIconSize(QSize(15, 15));
	QPixmap pixmap(25, 25);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(Qt::green);
	font.setPointSize(20);
	painter.setFont(font);
	painter.drawText(QPoint(5, 20), "A");
	QIcon colorIcon(pixmap);

	m_colorAction = cardEditMenuBar->addAction(colorIcon, "", Qt::Key_F8);
	m_colorAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Text Color"));
	connect(m_colorAction, &QAction::triggered, this, &AnkiCardEditor::onColorActionTriggered);

	pixmap.fill(Qt::darkYellow);
	painter.setPen(Qt::white);
	painter.drawText(QPoint(5, 20), "A");
	QIcon highlightIcon(pixmap);
	m_highlightAction = cardEditMenuBar->addAction(highlightIcon, "", Qt::Key_F9);
	m_highlightAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Highlight Color"));
	connect(m_highlightAction, &QAction::triggered, this, &AnkiCardEditor::onHighlightActionTriggered);

	cardEditMenuBar->addSeparator();

	m_resetAction = cardEditMenuBar->addAction("↺", Qt::Key_F12);
	m_resetAction->setToolTip(QCoreApplication::translate("AnkiCardEditor", "Reset"));
	connect(m_resetAction, &QAction::triggered, this, &AnkiCardEditor::onResetActionTriggered);

	return cardEditMenuBar;
}

void AnkiCardEditor::setFields(const QString& noteType)
{
	QFontMetrics metrics(font());

	mNoteTypeButton->setText(metrics.elidedText(noteType, Qt::ElideRight, mTextWidth));

	while(fieldsLayout->rowCount() > 0){
		fieldsLayout->removeRow(0);
	}

	for (const auto& fieldName : mNoteTypes[noteType]) {
		QTextEdit* fieldEdit = new QTextEdit();
		int height = setHeight(fieldEdit, 3);
		if (fieldHeight == 0) {
			fieldHeight = height;
		}
		
		
		QString elided = metrics.elidedText(fieldName, Qt::ElideRight, mTextWidth);

		fieldsLayout->addRow(elided, fieldEdit);
	}
	mCurrentNoteType = noteType;
}

void AnkiCardEditor::setDeck(const QString& deck)
{
	QFontMetrics metrics(font());
	QString elided = metrics.elidedText(deck, Qt::ElideRight, mTextWidth);

	mDeckButton->setText(elided);

	mCurrentDeck = deck;
}

void AnkiCardEditor::onBoldActionToggled(bool checked)
{
	for(int i = 0; i < fieldsLayout->rowCount(); i++){
		QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
		QFont boldFont = fieldEdit->currentFont();
		boldFont.setBold(checked);
		fieldEdit->setCurrentFont(boldFont);
	}
}

void AnkiCardEditor::onItalicActionToggled(bool checked)
{
	for (int i = 0; i < fieldsLayout->rowCount(); i++) {
		QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
		QFont italicFont = fieldEdit->currentFont();
		italicFont.setItalic(checked);
		fieldEdit->setCurrentFont(italicFont);
	}
}

void AnkiCardEditor::onUnderlineActionToggled(bool checked)
{
	for (int i = 0; i < fieldsLayout->rowCount(); i++) {
		QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
		QFont underlineFont = fieldEdit->currentFont();
		underlineFont.setUnderline(checked);
		fieldEdit->setCurrentFont(underlineFont);
	}
}

void AnkiCardEditor::onStrikeoutActionToggled(bool checked)
{
	for (int i = 0; i < fieldsLayout->rowCount(); i++) {
		QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
		QFont strikeoutFont = fieldEdit->currentFont();
		strikeoutFont.setStrikeOut(checked);
		fieldEdit->setCurrentFont(strikeoutFont);
	}
}

void AnkiCardEditor::onColorActionTriggered(bool checked)
{
	QColor color = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(0, QFormLayout::FieldRole)->widget())->textColor();
	QColorDialog* dlg = new QColorDialog(color, this);
	if (dlg->exec() == QDialog::Accepted) {
		color = dlg->selectedColor();
		for (int i = 0; i < fieldsLayout->rowCount(); i++) {
			QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
			fieldEdit->setTextColor(color);
		}
	}
}

void AnkiCardEditor::onHighlightActionTriggered(bool checked)
{
	QColor color = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(0, QFormLayout::FieldRole)->widget())->textBackgroundColor();
	QColorDialog* dlg = new QColorDialog(color, this);
	if (dlg->exec() == QDialog::Accepted) {
		color = dlg->selectedColor();
		for (int i = 0; i < fieldsLayout->rowCount(); i++) {
			QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
			fieldEdit->setTextBackgroundColor(color);
		}
	}
}

void AnkiCardEditor::onResetActionTriggered(bool checked)
{
	m_boldAction->setChecked(false);
	m_italicAction->setChecked(false);
	m_underlineAction->setChecked(false);
	m_strikeoutAction->setChecked(false);

	for (int i = 0; i < fieldsLayout->rowCount(); i++) {
		QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
		fieldEdit->setCurrentCharFormat(QTextCharFormat());
	}
	
}


void AnkiCardEditor::onNoteTypeButtonClicked()
{
	ListDialog dialog = ListDialog(this, ListDialogType::ListDialogTypeNoteType, mNoteTypes.keys());
	if (dialog.exec()) {
		if (!dialog.result.isEmpty() && !dialog.result[0].isEmpty()) {
			if (mNoteTypeButton->text() == dialog.result[0]) {
				return;
			}

			while (fieldsLayout->rowCount() > 0) {
				fieldsLayout->removeRow(0);
			}

			if (dialog.result.count() > 3) {
				mNewNoteTypes[dialog.result[0]] = { dialog.result[1], dialog.result[2] };
				for (int i = 3; i < dialog.result.count(); i++) {
					mNoteTypes[dialog.result[0]].append(dialog.result[i]);
				}
			}
			setFields(dialog.result[0]);
		}
	}
}

void AnkiCardEditor::onDeckButtonClicked()
{
	ListDialog dialog = ListDialog(this, ListDialogType::ListDialogTypeDeck, mDecks);
	if (dialog.exec()) {
		if (!dialog.result.isEmpty() && !dialog.result[0].isEmpty()) {
			setDeck(dialog.result[0]);
			if (dialog.result.count() > 1) {
				mNewDecks.insert(dialog.result[0]);
			}
		}
	}
}

int AnkiCardEditor::setHeight(QTextEdit* ptxt, int nRows)
{
	QTextDocument* pdoc = ptxt->document();
	QFontMetrics fm(pdoc->defaultFont());
	QMargins margins = ptxt->contentsMargins();
	int height = fm.lineSpacing() * nRows +
		(pdoc->documentMargin() + ptxt->frameWidth()) * 2 +
		margins.top() + margins.bottom();
	ptxt->setFixedHeight(height);
	return height;
}

QMap<QString, QString> AnkiCardEditor::getFields()
{
	QMap<QString, QString> fields;

	int i = 0;
	for (const auto& fieldName : mNoteTypes[mCurrentNoteType]) {
		if (i >= fieldsLayout->rowCount()) {
			break;
		}
		QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(i, QFormLayout::FieldRole)->widget());
		fields[fieldName] = fieldEdit->toHtml();
		i++;
	}
	return fields;
}

#include <QRegularExpression>
QStringList AnkiCardEditor::getTags()
{
	return mTagEdit->text().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}

QString AnkiCardEditor::getCurrentNoteType(bool& isNewNoteType)
{
	if (mNewNoteTypes.find(mCurrentNoteType) != mNewNoteTypes.end()) {
		isNewNoteType = true;
	}
	return mCurrentNoteType;
}

std::pair<QString, QString> AnkiCardEditor::getCurrentNoteTypeTemplates()
{
	auto it = mNewNoteTypes.find(mCurrentNoteType);
	if (it != mNewNoteTypes.end()) {
		return *it;
	}
	return {};
}

QString AnkiCardEditor::getCurrentDeck()
{
	return mCurrentDeck;
}

void AnkiCardEditor::onUpdateDeckNames(const QStringList& decks)
{
	for (const auto& deck : decks) {
		if (!mDecks.contains(deck)) {
			mDecks.append(deck);
		}
	}
}

void AnkiCardEditor::onUpdateNoteTypes(const QStringList& models, const QList<QStringList>& fieldsLists)
{
	int i = 0;
	for (const auto& model : models) {
		if (!mNoteTypes.contains(model)) {
			mNoteTypes[model] = fieldsLists[i];
		}
		++i;
	}
}

void AnkiCardEditor::onInsertDataIntoField(const QString& data, const QString& field)
{
	qsizetype index = mNoteTypes[mCurrentNoteType].indexOf(field);
	if (index == -1 || index >= fieldsLayout->rowCount()) {
		return;
	}
	QTextEdit* fieldEdit = dynamic_cast<QTextEdit*>(fieldsLayout->itemAt(index, QFormLayout::FieldRole)->widget());
	fieldEdit->insertPlainText(data);
}

void AnkiCardEditor::onModelCreated()
{
	mNewNoteTypes.remove(mCurrentNoteType);
}