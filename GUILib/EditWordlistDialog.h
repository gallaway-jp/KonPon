#pragma once
#include <qdialog.h>

#include "Settings.h"

#include <QString>

QT_BEGIN_NAMESPACE
class QWidget;
class QComboBox;
class QDialogButtonBox;
QT_END_NAMESPACE

class EditWordlistDialog :
    public QDialog
{
    Q_OBJECT
public:
    EditWordlistDialog(QWidget* parent, int WordlistType, const QString& wordlistname, int currentColor, Settings* settings);
    int result = 0;
private:
    Settings* m_settings = nullptr;
    QComboBox* mColorCombobox = nullptr;
    QDialogButtonBox* mButtonBox = nullptr;
private:
    QComboBox* createColorCombobox();
    void accept();
    void reject();
};