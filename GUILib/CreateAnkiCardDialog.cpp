#include "CreateAnkiCardDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>

#include "AnkiCardEditor.h"
#include "ListDialog.h"
#include "Settings.h"
#include "Word.h"

CreateAnkiCardDialog::CreateAnkiCardDialog(QWidget* parent, const Word& word, Settings* settings)
	: QDialog(parent),
	mKana(word.getKana()), mKanji(word.getKanji()),
	mAnkiConnect(settings->mAnki.address, QString::number(settings->mAnki.port))
{
	if (!word.getPitchAccents().empty()) {
		m_pitchAccents = std::to_string(*word.getPitchAccents().begin());
		std::for_each(std::next(word.getPitchAccents().begin()), word.getPitchAccents().end(), [this](int val) {
			m_pitchAccents.append(", ").append(std::to_string(val)); }
		);
	}
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	mainLayout->addLayout(horizontalLayout);

	constexpr int layoutSpacing = 25;
	QFormLayout* wordDataLayout = new QFormLayout();
	wordDataLayout->setHorizontalSpacing(layoutSpacing);
	horizontalLayout->addLayout(wordDataLayout);

	QPushButton* addKanaToFieldButton = new QPushButton(tr("Add to Field"));
	connect(addKanaToFieldButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onAddKanaToFieldButtonClicked);
	QPushButton* addKanjiToFieldButton = new QPushButton(tr("Add to Field"));
	connect(addKanjiToFieldButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onAddKanjiToFieldButtonClicked);

	wordDataLayout->addRow(tr("Kana"), createWordInfoWidget(mKana, addKanaToFieldButton));
	wordDataLayout->addRow(tr("Kanji"), createWordInfoWidget(mKanji, addKanjiToFieldButton));

	if (!m_pitchAccents.empty()) {
		QPushButton* addPitchAccentsToFieldButton = new QPushButton(tr("Add to Field"));
		connect(addPitchAccentsToFieldButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onAddPitchAccentsToFieldButtonClicked);
		wordDataLayout->addRow(tr("Pitch Accents"), createWordInfoWidget(m_pitchAccents, addPitchAccentsToFieldButton));
	}

	mAnkiCardEditor = new AnkiCardEditor();
	horizontalLayout->addWidget(mAnkiCardEditor);

	QPushButton* saveToLocalButton = new QPushButton(tr("Save to Local"));
	saveToLocalButton->setDisabled(true);
	connect(saveToLocalButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onSaveToLocalButtonClicked);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	buttonBox->addButton(saveToLocalButton, QDialogButtonBox::ActionRole);

	if (settings->mAnki.isAnkiConnectFeatureEnabled) {
		std::function<void(QStringList)> getDeckNamesSlot = [this](QStringList decks) { onGetDeckNames(decks); };
		mAnkiConnect.GetDeckNames(getDeckNamesSlot);

		std::function<void(QStringList)> getModelNamesSlot = [this](QStringList models) { onGetModelNames(models); };
		mAnkiConnect.getModelNames(getModelNamesSlot);

		mSendToAnkiButton = new QPushButton(tr("Send to Anki"));
		connect(mSendToAnkiButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onSendToAnkiButtonClicked);
		buttonBox->addButton(mSendToAnkiButton, QDialogButtonBox::ActionRole);
	}
	mainLayout->addWidget(buttonBox);

	connect(this, &CreateAnkiCardDialog::updateDeckNames, mAnkiCardEditor, &AnkiCardEditor::onUpdateDeckNames);
	connect(this, &CreateAnkiCardDialog::updateNoteTypes, mAnkiCardEditor, &AnkiCardEditor::onUpdateNoteTypes);
	connect(this, &CreateAnkiCardDialog::insertDataIntoField, mAnkiCardEditor, &AnkiCardEditor::onInsertDataIntoField);
	connect(this, &CreateAnkiCardDialog::modelCreated, mAnkiCardEditor, &AnkiCardEditor::onModelCreated);

	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

QWidget* CreateAnkiCardDialog::createWordInfoWidget(const std::string& data, QPushButton* addToFieldButton)
{
	QLabel* label = new QLabel(data.c_str());
	QWidget* widget = new QWidget();
	QHBoxLayout* kanaLayout = new QHBoxLayout();
	kanaLayout->addWidget(label);
	kanaLayout->addWidget(addToFieldButton);
	widget->setLayout(kanaLayout);
	return widget;
}

void CreateAnkiCardDialog::addWordInfoToField(const std::string& data)
{
	ListDialog dialog = ListDialog(this, ListDialogType::ListDialogTypeSelectField, mAnkiCardEditor->getFields().keys());
	dialog.exec();
	if (!dialog.result.isEmpty() && !dialog.result[0].isEmpty()) {
		emit insertDataIntoField(data.c_str(), dialog.result[0]);
	}
}

void CreateAnkiCardDialog::onAddKanaToFieldButtonClicked()
{
	addWordInfoToField(mKana);
}

void CreateAnkiCardDialog::onAddKanjiToFieldButtonClicked()
{
	addWordInfoToField(mKanji);
}

void CreateAnkiCardDialog::onAddPitchAccentsToFieldButtonClicked()
{
	addWordInfoToField(m_pitchAccents);
}

void CreateAnkiCardDialog::onSaveToLocalButtonClicked()
{
	// TODO implement save to local
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