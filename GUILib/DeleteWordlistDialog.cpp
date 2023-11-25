#include "DeleteWordlistDialog.h"

#include "QDialogButtonBox.h"
#include "QRadioButton"
#include "QLabel.h"
#include "QComboBox"
#include "QListWidget"
#include "QVBoxLayout"

DeleteWordlistDialog::DeleteWordlistDialog(const std::vector<std::string>& names, QDialog* parent)
	: QDialog(parent)
{
	if (!names.empty()) {
		mCurrentSelection = names[0];
	}

	QVBoxLayout* layout = new QVBoxLayout(this);
	mListNames = new QListWidget();
	for (const auto& name : names) {
		mListNames->addItem(name.c_str());
	}
	QRadioButton* moveRadioButton = new QRadioButton(tr("Move words to the below list"));
	QRadioButton* deleteRadioButton = new QRadioButton(tr("Delete words contained in list"));
	mDestinationCombo = new QComboBox();
	//mDestinationCombo->setInsertPolicy(QComboBox::InsertAlphabetically);
	//mDestinationCombo->insertItem()
	mDestinationCombo->addItem(tr("Unknown List"), 1);
	mDestinationCombo->addItem(tr("Known List"), 2);
	bool isFirst = true;
	for (const auto& name : names) {
		// Skip first item since it will be selected for deletion by default
		if (isFirst) {
			isFirst = false;
			continue;
		}

		mDestinationCombo->addItem(name.c_str(), 3);
	}
	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	layout->addWidget(mListNames);
	layout->addWidget(deleteRadioButton);
	layout->addWidget(moveRadioButton);
	layout->addWidget(mDestinationCombo);
	layout->addWidget(mButtonBox);

	moveRadioButton->setChecked(true);
	deleteRadioButton->setChecked(false);

	connect(mListNames, &QListWidget::itemSelectionChanged, this, &DeleteWordlistDialog::onSelectionChanged);
	connect(moveRadioButton, &QAbstractButton::toggled, this, &DeleteWordlistDialog::onRadioButtonPressed);
	connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	setWindowTitle(tr("Delete Wordlist"));
	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void DeleteWordlistDialog::onRadioButtonPressed(bool bMove)
{
	mMove = bMove;
	toggleMoveDestinationCombobox();
}

void DeleteWordlistDialog::toggleMoveDestinationCombobox()
{
	if (mMove) {
		if (!mDestinationCombo->isVisible()) {
			mDestinationCombo->setVisible(true);
		}
		return;
	}

	mDestinationCombo->setVisible(false);
}

void DeleteWordlistDialog::onSelectionChanged()
{
	for (const QListWidgetItem* item : mListNames->selectedItems()) {
		// Search from after builtin wordlist names
		int insertIndex = mDestinationCombo->findData(3);
		if (insertIndex == -1) {
			// There should always be at least 1 custom list in the combobox
			// assuming there are 2 or more custom lists
			return;
		}
		
		// Place the previously selected item back into the combobox
		while (insertIndex < mDestinationCombo->count() && mDestinationCombo->itemText(insertIndex) < QString(mCurrentSelection.c_str())) {
			insertIndex++;
		}
		mDestinationCombo->insertItem(insertIndex, mCurrentSelection.c_str(), 3);

		int itemToRemove = mDestinationCombo->findText(item->text());
		QVariant data = mDestinationCombo->itemData(itemToRemove);
		while (data.isValid() && data.toInt() != 3) {
			// Avoid removing removing builtin unknown/known lists
			itemToRemove++;
			data = mDestinationCombo->itemData(itemToRemove);
			if (data.isValid() && data.toInt() == 3) {
				if (mDestinationCombo->itemText(itemToRemove) == item->text()) {
					break;
				}
			}
		}
		
		if (data.isValid() && data.toInt() == 3) {
			mDestinationCombo->removeItem(itemToRemove);
		}
		mCurrentSelection = item->text().toStdString();
		break;
	}
}

void DeleteWordlistDialog::accept()
{
	result.first = mCurrentSelection;

	// TODO replace magic number destinationType with enum
	if (mMove) {
		int destinationType = mDestinationCombo->currentData().toInt();
		result.second = { destinationType, destinationType == 3 ? mDestinationCombo->currentText().toStdString() : ""};
	}
	QDialog::accept();
}

void DeleteWordlistDialog::reject()
{
	QDialog::reject();
}