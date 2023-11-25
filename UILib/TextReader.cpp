#include "TextReader.h"

TextReader::TextReader()
    : QTextEdit()
{
    setReadOnly(true);
    setDocumentTitle("Test Title");
    
}

// Make arrow keys also move cursor/select
void TextReader::keyPressEvent(QKeyEvent* e)
{
    int nPos = textCursor().position();
    int nAnchor = textCursor().anchor();
    bool bShiftPressed = e->modifiers() & Qt::ShiftModifier;
    bool bAltPressed = e->modifiers() & Qt::AltModifier;
    switch (e->key()) {
    case Qt::Key_Left:
    {
        if (bAltPressed) {
            // go to next word
            textCursor().setPosition(nPos - 10, bShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
            return;
        }

        if (nPos == 0) {
            // go to previous page

            if (bShiftPressed) {
                // keep selection even after changing to previous page
                QString text = textCursor().selectedText();

                if (!text.isEmpty()) {
                    int selStart = textCursor().selectionStart();
                    int selEnd = textCursor().selectionEnd();
                }
            }
            return;
        }

        moveCursor(QTextCursor::Left, bShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        return;
    }
    case Qt::Key_Right:
    {
        if (bAltPressed) {
            // go to next word
            textCursor().setPosition(nPos + 10, bShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
            return;
        }
        moveCursor(QTextCursor::Right, bShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        return;
    }
    case Qt::Key_Up:
    {
        moveCursor(QTextCursor::Up, bShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        return;
    }
    case Qt::Key_Down:
    {
        moveCursor(QTextCursor::Down, bShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        return;
    }
    }
    nPos = nPos = textCursor().position();
    QTextEdit::keyPressEvent(e);
}

TextReader::~TextReader()
{
    ;
}