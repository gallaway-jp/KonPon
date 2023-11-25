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

AddNoteType::AddNoteType(QWidget* parent)
	: QDialog(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QFormLayout* editLayout = new QFormLayout();
	layout->addLayout(editLayout);

	mNoteTypeLineEdit = new QLineEdit();
	editLayout->addRow(tr("Note Type: "), mNoteTypeLineEdit);

	mListWidget = new QListWidget();
	layout->addWidget(new QLabel(tr("Fields")));

	QHBoxLayout* listLayout = new QHBoxLayout();
	listLayout->addWidget(mListWidget);
	layout->addLayout(listLayout);

	QVBoxLayout* fieldButtonLayout = new QVBoxLayout();
	listLayout->addLayout(fieldButtonLayout);

	QPushButton* createFieldButton = new QPushButton(tr("Add"));
	connect(createFieldButton, &QPushButton::clicked, this, &AddNoteType::onCreateFieldButtonClicked);
	QPushButton* deleteFieldButton = new QPushButton(tr("Delete"));
	connect(deleteFieldButton, &QPushButton::clicked, this, &AddNoteType::onDeleteFieldButtonClicked);
	
	fieldButtonLayout->addWidget(createFieldButton);
	fieldButtonLayout->addWidget(deleteFieldButton);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
	layout->addWidget(buttonBox);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AddNoteType::accept()
{
	noteType = mNoteTypeLineEdit->text();
	for (int i = 0; i < mListWidget->count(); i++) {
		fields.append(mListWidget->item(i)->text());
	}
	QDialog::accept();
}

void AddNoteType::onCreateFieldButtonClicked()
{
	QString text = QInputDialog::getText(this, tr("Add a Field"), "");
	if (!text.isEmpty()) {
		if (!mListWidget->findItems(text, Qt::MatchExactly).empty()) {
			QMessageBox::information(this, tr("Item already exists"), tr("Item already exists in list!"));
			return;
		}
		mListWidget->addItem(text);
	}
}

void AddNoteType::onDeleteFieldButtonClicked()
{
	for (auto item : mListWidget->selectedItems()) {
		delete mListWidget->takeItem(mListWidget->row(item));
	}
}