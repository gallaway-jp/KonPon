#pragma once
#include <QTextEdit>
#include <QKeyEvent>

class TextReader : public QTextEdit
{

public:
    TextReader();
    ~TextReader();

private:
    void keyPressEvent(QKeyEvent* e);

};

