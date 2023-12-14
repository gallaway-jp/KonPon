#include "CreateAnkiCardDialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QSizePolicy>
#include <QMessageBox>

#include "DragWidget.h"
#include "AnkiCardEditor.h"
#include "Settings.h"

CreateAnkiCardDialog::CreateAnkiCardDialog(QWidget* parent, const std::string& kana, const std::string& kanji, Settings* settings)
	: QDialog(parent), mKana(kana), mKanji(kanji), mAnkiConnect(settings->mAnki.address, QString::number(settings->mAnki.port))
{
	setAcceptDrops(true);
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	mainLayout->addLayout(horizontalLayout);

	constexpr int layoutSpacing = 25;
	QFormLayout* wordDataLayout = new QFormLayout();
	wordDataLayout->setHorizontalSpacing(layoutSpacing);
	horizontalLayout->addLayout(wordDataLayout);

	QLabel* kanaLabel = new QLabel(kana.c_str());
	QPushButton* addKanaToFieldButton = new QPushButton(tr("Add to Field"));
	connect(addKanaToFieldButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onAddKanaToFieldButtonClicked);
	QWidget* kanaWidget = new QWidget();
	QHBoxLayout* kanaLayout = new QHBoxLayout();
	kanaLayout->addWidget(kanaLabel);
	kanaLayout->addWidget(addKanaToFieldButton);
	kanaWidget->setLayout(kanaLayout);

	QLabel* kanjiLabel = new QLabel(kanji.c_str());
	QPushButton* addKanjiToFieldButton = new QPushButton(tr("Add to Field"));
	connect(addKanjiToFieldButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onAddKanjiToFieldButtonClicked);
	QWidget* kanjiWidget = new QWidget();
	QHBoxLayout* kanjiLayout = new QHBoxLayout();
	kanjiLayout->addWidget(kanjiLabel);
	kanjiLayout->addWidget(addKanjiToFieldButton);
	kanjiWidget->setLayout(kanjiLayout);

	wordDataLayout->addRow(tr("Kana"), kanaWidget);
	wordDataLayout->addRow(tr("Kanji"), kanjiWidget);
#if 0 //TODO Make word data draggable to each field
	wordDataLayout->addWidget(new DragWidget());
#endif

	std::function<void(QStringList)> getDeckNamesSlot = [this](QStringList decks) { onGetDeckNames(decks); };
	mAnkiConnect.GetDeckNames(getDeckNamesSlot);

	std::function<void(QStringList)> getModelNamesSlot = [this](QStringList models) { onGetModelNames(models); };
	mAnkiConnect.getModelNames(getModelNamesSlot);

	mAnkiCardEditor = new AnkiCardEditor();
	horizontalLayout->addWidget(mAnkiCardEditor);

	QPushButton* saveToLocalButton = new QPushButton(tr("Save to Local"));
	saveToLocalButton->setDisabled(true);
	connect(saveToLocalButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onSaveToLocalButtonClicked);
	mSendToAnkiButton = new QPushButton(tr("Send to Anki"));
	connect(mSendToAnkiButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onSendToAnkiButtonClicked);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	buttonBox->addButton(saveToLocalButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(mSendToAnkiButton, QDialogButtonBox::ActionRole);
	mainLayout->addWidget(buttonBox);

	connect(this, &CreateAnkiCardDialog::updateDeckNames, mAnkiCardEditor, &AnkiCardEditor::onUpdateDeckNames);
	connect(this, &CreateAnkiCardDialog::updateNoteTypes, mAnkiCardEditor, &AnkiCardEditor::onUpdateNoteTypes);
	connect(this, &CreateAnkiCardDialog::insertDataIntoField, mAnkiCardEditor, &AnkiCardEditor::onInsertDataIntoField);
	connect(this, &CreateAnkiCardDialog::modelCreated, mAnkiCardEditor, &AnkiCardEditor::onModelCreated);

	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

#include "ListDialog.h"
void CreateAnkiCardDialog::onAddKanaToFieldButtonClicked()
{
	ListDialog dialog = ListDialog(this, ListDialogType::ListDialogTypeSelectField, mAnkiCardEditor->getFields().keys());
	dialog.exec();
	if (!dialog.result.isEmpty() && !dialog.result[0].isEmpty()) {
		emit insertDataIntoField(mKana.c_str(), dialog.result[0]);
	}
}

void CreateAnkiCardDialog::onAddKanjiToFieldButtonClicked()
{
	ListDialog dialog = ListDialog(this, ListDialogType::ListDialogTypeSelectField, mAnkiCardEditor->getFields().keys());
	dialog.exec();
	if (!dialog.result.isEmpty() && !dialog.result[0].isEmpty()) {
		emit insertDataIntoField(mKanji.c_str(), dialog.result[0]);
	}
}

void CreateAnkiCardDialog::onSaveToLocalButtonClicked()
{
	int a = 0;
}

void CreateAnkiCardDialog::onSendToAnkiButtonClicked()
{
	mSendToAnkiButton->setDisabled(true);
	
	bool isNewNoteType = false;
	QString deck = mAnkiCardEditor->getCurrentDeck();
	QString noteType = mAnkiCardEditor->getCurrentNoteType(isNewNoteType);
	if (isNewNoteType) {
		std::function<void(bool)> slot = [this](bool created) {onModelCreated(created); };
		NewCardTypeData newCardTypeData;
		newCardTypeData.fields = mAnkiCardEditor->getFields().keys();
		auto [frontTemplate, backTemplate] = mAnkiCardEditor->getCurrentNoteTypeTemplates();
		newCardTypeData.frontTemplate = frontTemplate;
		newCardTypeData.backTemplate = backTemplate;
		mAnkiConnect.createModel(noteType, newCardTypeData, slot);
		return;
	}

	std::vector<std::pair<QString, QString>> fields;
	for (const auto& [fieldName, fieldData] : mAnkiCardEditor->getFields().asKeyValueRange()) {
		fields.push_back({ fieldName, fieldData });
	}
	std::function<void(bool)> slot = [this](bool added) {onNoteAdded(added); };
	mAnkiConnect.addNote(deck, noteType, fields, mAnkiCardEditor->getTags(), slot);
}

void CreateAnkiCardDialog::onGetDeckNames(const QStringList& decks)
{
	emit updateDeckNames(decks);
}

void CreateAnkiCardDialog::onGetModelNames(const QStringList& models)
{
	mModels = models;
	std::function<void(QList<QStringList>)> getModelsFieldNamesSlot = [this](QList<QStringList> fieldsLists) { onGetModelsFieldNames(fieldsLists); };
	mAnkiConnect.getModelsFieldNames(models, getModelsFieldNamesSlot);
}

void CreateAnkiCardDialog::onGetModelsFieldNames(const QList<QStringList>& fieldsLists)
{
	emit updateNoteTypes(mModels, fieldsLists);
}

void CreateAnkiCardDialog::onNoteAdded(bool bAdded)
{
	mSendToAnkiButton->setEnabled(true);

	if (!bAdded) {
		QMessageBox::information(this, tr("Could not create note"), tr("Failed to create note!"));
	}
}

void CreateAnkiCardDialog::onModelCreated(bool created)
{
	if (!created) {
		QMessageBox::information(this, tr("Could not create note type"), tr("Failed to create note type!"));
		mSendToAnkiButton->setEnabled(true);
		return;
	}

	bool isNew = false;
	QString deck = mAnkiCardEditor->getCurrentDeck();
	QString noteType = mAnkiCardEditor->getCurrentNoteType(isNew);
	emit modelCreated();

	std::function<void(bool)> slot = [this](bool added) {onNoteAdded(added); };

	std::vector<std::pair<QString, QString>> fields;
	for (const auto& [fieldName, fieldData] : mAnkiCardEditor->getFields().asKeyValueRange()) {
		fields.push_back({ fieldName, fieldData });
	}

	mAnkiConnect.addNote(deck, noteType, fields, mAnkiCardEditor->getTags(), slot);
}