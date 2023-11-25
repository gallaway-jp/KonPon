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

#include "DragWidget.h"
#include "AnkiCardEditor.h"

CreateAnkiCardDialog::CreateAnkiCardDialog(QWidget* parent, const std::string& kana, const std::string& kanji)
	: QDialog(parent)
{
	

	setAcceptDrops(true);
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	mainLayout->addLayout(horizontalLayout);
	// select card type and deck
	// default card type has:
	// front
	// back
	// tags
	constexpr int layoutSpacing = 25;
	QFormLayout* wordDataLayout = new QFormLayout();
	wordDataLayout->setHorizontalSpacing(layoutSpacing);
	horizontalLayout->addLayout(wordDataLayout);

	QLabel* kanaLabel = new QLabel(kana.c_str());
	wordDataLayout->addRow(tr("Kana"), kanaLabel);
	QLabel* kanjiLabel = new QLabel(kanji.c_str());
	wordDataLayout->addRow(tr("Kanji"), kanjiLabel);
	wordDataLayout->addWidget(new DragWidget());


	mAnkiCardEditor = new AnkiCardEditor(nullptr);
	horizontalLayout->addWidget(mAnkiCardEditor);

	QPushButton* saveToLocalButton = new QPushButton(tr("Save to Local"));
	connect(saveToLocalButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onSaveToLocalButtonClicked);
	mSendToAnkiButton = new QPushButton(tr("Send to Anki"));
	connect(mSendToAnkiButton, &QPushButton::clicked, this, &CreateAnkiCardDialog::onSendToAnkiButtonClicked);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	buttonBox->addButton(saveToLocalButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(mSendToAnkiButton, QDialogButtonBox::ActionRole);
	mainLayout->addWidget(buttonBox);

	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

void CreateAnkiCardDialog::onSaveToLocalButtonClicked()
{
	int a = 0;
}

// TODO: Properly retrieve template data
void CreateAnkiCardDialog::onSendToAnkiButtonClicked()
{
	mSendToAnkiButton->setDisabled(true);
	
	bool isNew = false;
	QString deck = mAnkiCardEditor->getCurrentDeck(isNew);
	QString noteType = mAnkiCardEditor->getCurrentNoteType(isNew);
	if (isNew) {
		std::function<void(bool)> slot = [this](bool created) {onDeckCreated(created); };
		std::vector<std::pair<QString, CardTemplateFieldType>> fields;
		bool first = true;
		for (const auto& [fieldName, fieldData] : mAnkiCardEditor->getFields().asKeyValueRange()) {
			if (first) {
				fields.push_back({ fieldName, CardTemplateFieldType::FrontOnly });
				first = false;
				continue;
			}
			fields.push_back({ fieldName, CardTemplateFieldType::BackOnly });
		}
		mAnkiConnect.createModel(noteType, fields, slot);
		return;
	}

	std::vector<std::pair<QString, QString>> fields;
	for (const auto& [fieldName, fieldData] : mAnkiCardEditor->getFields().asKeyValueRange()) {
		fields.push_back({ fieldName, fieldData });
	}
	std::function<void(bool)> slot = [this](bool added) {onNoteAdded(added); };
	mAnkiConnect.addNote(deck, noteType, fields, mAnkiCardEditor->getTags(), slot);
	/*mAnkiConnect.addNote("createDeckTestDeck", "_TestType_", { { "表面", "FieldData0112"}, { "裏面", "FieldData0112"} }, {}, slot);*/
}

void CreateAnkiCardDialog::onNoteAdded(bool bAdded)
{
	mSendToAnkiButton->setEnabled(true);
}

void CreateAnkiCardDialog::onDeckCreated(bool created)
{
	bool isNew = false;
	QString deck = mAnkiCardEditor->getCurrentDeck(isNew);
	QString noteType = mAnkiCardEditor->getCurrentNoteType(isNew);
	std::function<void(bool)> slot = [this](bool added) {onNoteAdded(added); };

	std::vector<std::pair<QString, QString>> fields;
	for (const auto& [fieldName, fieldData] : mAnkiCardEditor->getFields().asKeyValueRange()) {
		fields.push_back({ fieldName, fieldData });
	}

	mAnkiConnect.addNote(deck, noteType, fields, mAnkiCardEditor->getTags(), slot);
}

#include <QDropEvent>
#include <QMimeData>
//void CreateAnkiCardDialog::dragEnterEvent(QDragEnterEvent* event)
//{
//	event->accept();
//}
//
//void CreateAnkiCardDialog::dropEvent(QDropEvent* event)
//{
//	if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
//		QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
//		QDataStream dataStream(&itemData, QIODevice::ReadOnly);
//
//		//QPixmap pixmap;
//		QString text;
//		QPoint offset;
//		dataStream /*>> pixmap*/ >> text >> offset;
//
//		//frontTextEdit->setText(text);
//		//QString html1 = "<div style=\"border - radius: 10px; background: beige; padding: 10px; \">";
//		//QString html2 = "</div>";
//		QString html1 = "<b>";
//		QString html2 = "</b>";
//		frontTextEdit->setHtml(html1 + text + html2);
//		
//	}
//	QDialog::dropEvent(event);
//}