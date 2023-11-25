#pragma once
#include <qdialog.h>

#include <QString>

class QWidget;
class QComboBox;
class QDialogButtonBox;

class EditWordlistDialog :
    public QDialog
{
    Q_OBJECT
public:
    EditWordlistDialog(QWidget* parent, int WordlistType, const QString& wordlistname, int currentColor);
    int result = 0;
private:
    QComboBox* mColorCombobox = nullptr;
    QComboBox* createColorCombobox();
    QDialogButtonBox* mButtonBox = nullptr;
    void accept();
    void reject();
};