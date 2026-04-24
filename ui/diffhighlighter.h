#pragma once

#include <QPlainTextEdit>
#include <QList>
#include <QTextEdit>
#include <QColor>

#include "models/diffmodel.h"

class DiffHighlighter
{
public:
    static void apply(QPlainTextEdit* leftEditor,
                      QPlainTextEdit* rightEditor,
                      const DiffResult& result);

    static void clear(QPlainTextEdit* leftEditor,
                      QPlainTextEdit* rightEditor);

private:
    static QList<QTextEdit::ExtraSelection> buildLeftSelections(
        QPlainTextEdit* editor,
        const DiffResult& result);

    static QList<QTextEdit::ExtraSelection> buildRightSelections(
        QPlainTextEdit* editor,
        const DiffResult& result);

    static QTextEdit::ExtraSelection makeLineSelection(
        QPlainTextEdit* editor,
        int             lineIndex,
        const QColor&   bgColor,
        const QColor&   fgColor);

    DiffHighlighter() = delete;
};
