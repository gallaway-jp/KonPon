#include "CustomCheckboxDialog.h"

#include <QScrollArea>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

CustomCheckboxDialog::CustomCheckboxDialog(QWidget* parent, const std::vector<std::string>& customWordlistNames, const std::unordered_set<std::string> highlightedCustomWordlists)
	: QDialog(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QVBoxLayout* checkboxLayout = new QVBoxLayout();
	QScrollArea* scrollArea = new QScrollArea();
	layout->addWidget(scrollArea);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setWidgetResizable(true);
	scrollArea->setMinimumWidth(250);
	scrollArea->setMinimumHeight(50);
	QWidget* widget = new QWidget();
	widget->setLayout(checkboxLayout);
	scrollArea->setWidget(widget);

	for (const auto& customWordlist : customWordlistNames) {
		QCheckBox* cb = new QCheckBox(customWordlist.c_str());
		checkboxLayout->addWidget(cb);
		if (highlightedCustomWordlists.count(customWordlist) != 0) {
			cb->setChecked(true);
		}
		connect(cb, &QCheckBox::toggled, this, [this, customWordlist](bool checked) {toggleCustomCheckbox(customWordlist, checked); });
	}

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
	layout->addWidget(buttonBox);

	setWindowTitle(tr("Select custom wordlists to highlight"));
	setMaximumWidth(300);
	setMaximumHeight(300);
	//this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}