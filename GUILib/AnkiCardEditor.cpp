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
	tagsLayout->addRow(tr("Tags"), mTagEdit);
}

void AnkiCardEditor::setTextWidth()
{
	/*QLabel* labeltest = new QLabel(mCurrentDeck);
	mTextWidth = labeltest->width();
	delete labeltest;*/
	mTextWidth = 50;
}

void AnkiCardEditor::createNoteDeckButtons()
{
	QHBoxLayout* deckNoteLayout = new QHBoxLayout();
	mLayout->addLayout(deckNoteLayout);

	QFormLayout* noteTypeLayout = new QFormLayout();
	deckNoteLayout->addLayout(noteTypeLayout);
	mNoteTypeButton = new QPushButton(mCurrentNoteType);
	noteTypeLayout->addRow(tr("Note Type"), mNoteTypeButton);
	QFormLayout* deckLayout = new QFormLayout();
	deckNoteLayout->addLayout(deckLayout);
	mDeckButton = new QPushButton(mCurrentDeck);
	deckLayout->addRow(tr("Deck Type"), mDeckButton);

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
	//fieldsLayout->setSpacing(0);
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
	
	QAction* boldAction = cardEditMenuBar->addAction("B", QKeySequence::Bold);
	//boldAction->setStatusTip(tr("Bold"));
	boldAction->setToolTip(tr("Bold"));
	boldAction->setCheckable(true);
	boldAction->setChecked(false);
	QFont font = QFont(boldAction->font());
	font.setBold(true);
	boldAction->setFont(font);
	connect(boldAction, &QAction::toggled, this, &AnkiCardEditor::onBoldActionToggled);

	QAction* italicAction = cardEditMenuBar->addAction("I", QKeySequence::Italic);
	italicAction->setToolTip(tr("Italic"));
	italicAction->setCheckable(true);
	italicAction->setChecked(false);
	font.setBold(false);
	font.setItalic(true);
	italicAction->setFont(font);
	connect(italicAction, &QAction::toggled, this, &AnkiCardEditor::onItalicActionToggled);

	QAction* underlineAction = cardEditMenuBar->addAction("U", QKeySequence::Underline);
	underlineAction->setToolTip(tr("Underline"));
	underlineAction->setCheckable(true);
	underlineAction->setChecked(false);
	font.setItalic(false);
	font.setUnderline(true);
	underlineAction->setFont(font);
	connect(underlineAction, &QAction::toggled, this, &AnkiCardEditor::onUnderlineActionToggled);

	QAction* strikeoutAction = cardEditMenuBar->addAction("S", Qt::Key_F7);
	strikeoutAction->setToolTip(tr("Strikeout"));
	strikeoutAction->setCheckable(true);
	strikeoutAction->setChecked(false);
	font.setUnderline(false);
	font.setStrikeOut(true);
	strikeoutAction->setFont(font);
	connect(strikeoutAction, &QAction::toggled, this, &AnkiCardEditor::onStrikeoutActionToggled);

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

	QAction* colorAction = cardEditMenuBar->addAction(colorIcon, "", Qt::Key_F8);
	colorAction->setToolTip(tr("Text Color"));
	connect(colorAction, &QAction::triggered, this, &AnkiCardEditor::onColorActionTriggered);

	pixmap.fill(Qt::darkYellow);
	painter.setPen(Qt::white);
	painter.drawText(QPoint(5, 20), "A");
	QIcon highlightIcon(pixmap);
	QAction* highlightAction = cardEditMenuBar->addAction(highlightIcon, "", Qt::Key_F9);
	colorAction->setToolTip(tr("Highlight Color"));
	connect(highlightAction, &QAction::triggered, this, &AnkiCardEditor::onHighlightActionTriggered);

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

			if (dialog.result.count() > 1) {
				mNewNoteTypes.insert(dialog.result[0]);
				for (int i = 1; i < dialog.result.count(); i++) {
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

QString AnkiCardEditor::getCurrentDeck(bool& isNewDeck)
{
	if (mNewDecks.find(mCurrentDeck) != mNewDecks.end()) {
		isNewDeck = true;
	}
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