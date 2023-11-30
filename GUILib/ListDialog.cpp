#include "ListDialog.h"

#include "AddNoteType.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>

ListDialog::ListDialog(QWidget* parent, ListDialogType listDialogType, const QStringList& items)
	: QDialog(parent), mListDialogType(listDialogType)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	mListWidget = new QListWidget();

	if (!items.isEmpty()) {
		mListWidget->addItems(items);
	}

	layout->addWidget(mListWidget);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::Cancel, Qt::Horizontal);
	layout->addWidget(buttonBox);
	connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	if (mListDialogType == ListDialogType::ListDialogTypeNoteType
		|| mListDialogType == ListDialogType::ListDialogTypeDeck) {
		QPushButton* createButton = new QPushButton(tr("Create"));
		buttonBox->addButton(createButton, QDialogButtonBox::ActionRole);
		connect(createButton, &QPushButton::clicked, this, &ListDialog::onCreateButtonClicked);
	}
}

void ListDialog::accept()
{
	const QListWidgetItem* selectedItem = nullptr;
	for (const auto item : mListWidget->selectedItems()) {
		selectedItem = item;
		break;
	}

	if (!selectedItem) {
		return;
	}

	result.append(selectedItem->text());

	if (mListDialogType == ListDialogType::ListDialogTypeNoteType) {
		QVariant itemData = selectedItem->data(Qt::UserRole);
		if (itemData.isValid()) {
			for (const auto& field : itemData.toStringList()) {
				result.append(field);
			}
		}
	}
	else if(mListDialogType == ListDialogType::ListDialogTypeDeck) {
		QVariant itemData = selectedItem->data(Qt::UserRole);
		if (itemData.isValid() && itemData.toBool()) {
			// Empty string in list signifies new deck
			result.append("");
		}
	}

	QDialog::accept();
}

void ListDialog::onCreateButtonClicked()
{
	switch (mListDialogType)
	{
	case ListDialogType::ListDialogTypeNoteType:
		getNewNoteType();
		break;
	case ListDialogType::ListDialogTypeDeck:
		getNewDeckName();
		break;
	default:
		break;
	}
}

void ListDialog::getNewDeckName()
{
	QString text = QInputDialog::getText(this, tr("Create a new Deck"), "");
	if (!text.isEmpty()) {
		if (!mListWidget->findItems(text, Qt::MatchExactly).empty()) {
			QMessageBox::information(this, tr("Item already exists"), tr("Item already exists in list!"));
			return;
		}

		QListWidgetItem* item = new QListWidgetItem(text);
		item->setData(Qt::UserRole, true);
		mListWidget->addItem(item);
	}
}

void ListDialog::getNewNoteType()
{
	AddNoteType addNoteTypeDialog = AddNoteType(this);
	addNoteTypeDialog.exec();
	if (!addNoteTypeDialog.noteType.isEmpty() && !addNoteTypeDialog.fields.isEmpty()) {
		if (!mListWidget->findItems(addNoteTypeDialog.noteType, Qt::MatchExactly).empty()) {
			QMessageBox::information(this, tr("Item already exists"), tr("Item already exists in list!"));
			return;
		}

		QListWidgetItem* item = new QListWidgetItem(addNoteTypeDialog.noteType);
		item->setData(Qt::UserRole, { addNoteTypeDialog.fields });
		mListWidget->addItem(item);
	}
}
