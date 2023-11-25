#pragma once
#include <qdialog.h>

#include <string>
#include <unordered_set>
class Settings;
class Wordlists;

class CustomCheckboxDialog :
    public QDialog
{
    Q_OBJECT
public:
    CustomCheckboxDialog(QWidget* parent, const std::vector<std::string>& customWordlistNames, const std::unordered_set<std::string> highlightedCustomWordlists);
private:
signals:
    void toggleCustomCheckbox(const std::string& name, bool checked);
};