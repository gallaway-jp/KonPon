#include "EditWordlistDialog.h"

#include <vector>
#include <string>
#include <map>

#include "QFormLayout"
#include "QComboBox"
#include "QColor"
#include "QString"
#include <QDialogButtonBox>

EditWordlistDialog::EditWordlistDialog(QWidget* parent, int WordlistType, const QString& wordlistname, int currentColor, Settings* settings)
	: QDialog(parent), m_settings(settings)
{
    switch (WordlistType)
    {
    case 1:
        setWindowTitle(tr("Edit Unknown List Settings"));
        break;
    case 2:
        setWindowTitle(tr("Edit Known List Settings"));
        break;
    case 3:
        setWindowTitle(tr("Edit ") + wordlistname + tr(" Settings"));
        break;
    default:
        break;
    }


	QFormLayout* layout = new QFormLayout(this);
	layout->setHorizontalSpacing(50);

    mColorCombobox = createColorCombobox();
    mColorCombobox->setCurrentIndex(currentColor);
	layout->addRow(tr("Text Highlight Color"), mColorCombobox);

    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(mButtonBox);

    connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void EditWordlistDialog::accept()
{
    // TODO replace magic number color with enum
    result = mColorCombobox->currentIndex();

    QDialog::accept();
}

void EditWordlistDialog::reject()
{
    QDialog::reject();
}

QString getColorName(const QColor& color);
QComboBox* EditWordlistDialog::createColorCombobox()
{
	QComboBox* combobox = new QComboBox();

	std::vector<std::pair<std::pair<Qt::GlobalColor, Qt::GlobalColor>, QString>> colors = {
        {{Qt::transparent, Qt::transparent}, tr("No Color")},
        {{Qt::lightGray, Qt::darkGray}, tr("Gray")},
        {{Qt::red, Qt::darkRed}, tr("Red")},
        {{Qt::green, Qt::darkGreen}, tr("Green")},
        {  {Qt::blue, Qt::darkBlue}, tr("Blue")},
        {{Qt::yellow, Qt::darkYellow}, tr("Yellow")},
        {{Qt::cyan, Qt::darkCyan}, tr("Cyan")},
        {{Qt::magenta, Qt::darkMagenta}, tr("Magenta")},
	};

    for (uint8_t i = 0; i < colors.size(); i++) {
		QColor qcolor(m_settings->ui.isDarkTheme() ? colors[i].first.second
            : colors[i].first.first);
        
        combobox->insertItem(i, colors[i].second);
        combobox->setItemData(i, qcolor, Qt::DecorationRole);
	}

	return combobox;
}

// Lookup class that is only accessible from getColorName free function.
class NamedQColorLookup
{
private:
    NamedQColorLookup()
    {
        auto keyList = QColor::colorNames();

        // Simple implementation for filling _keys and _values using std::map.
        // Alternatively, sort two vectors at once, for example like
        // https://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of
        // But that's less readable and (since it's only done once) has no meaningful performance impact.
        std::map<std::uint64_t, QString> colorMap;
        for (const auto& key : keyList)
            colorMap.emplace(QColor(key).rgba64(), key);

        // Convert to faster and smaller vector lookup.
        _keys.reserve(colorMap.size());
        _values.reserve(colorMap.size());
        for (const auto& [key, value] : colorMap)
        {
            _keys.emplace_back(key);
            _values.emplace_back(value);
        }
    }

    QString getName(const QColor& color) const
    {
        auto rgba = color.rgba64();
        // Binary search for the RGBA value.
        auto [notLessThan, greaterThan] = std::equal_range(_keys.begin(), _keys.end(), rgba);

        // If this is not a named color, return the RGB code instead.
        if (notLessThan == greaterThan)
            return color.name();

        // We found a matching ARGB value, obtain its index.
        auto index = std::distance(_keys.begin(), notLessThan);
        return _values[index];
    }

    std::vector<std::uint64_t> _keys;
    std::vector<QString> _values; // ...or some kind of string view if you want.

    friend QString getColorName(const QColor& color);
};

// The interface for color -> name lookups.
QString getColorName(const QColor& color)
{
    static NamedQColorLookup lookup;
    return lookup.getName(color);
}