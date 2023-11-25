#pragma once
#include <qdialog.h>

#include <string>
#include <vector>

class QDialogButtonBox;
class QListWidget;
class QComboBox;

class DeleteWordlistDialog :
    public QDialog
{
    Q_OBJECT
public:
    DeleteWordlistDialog(const std::vector<std::string>& names, QDialog* parent = nullptr);
    std::pair<std::string, std::pair<int, std::string>> result = { "", { 0, "" } };
private:
    void onRadioButtonPressed(bool bMove);
    void toggleMoveDestinationCombobox();
    void onSelectionChanged();
    bool mMove = true;
    std::string mCurrentSelection;
    void accept();
    void reject();
    QListWidget* mListNames;
    QComboBox* mDestinationCombo;
    QDialogButtonBox* mButtonBox;
};