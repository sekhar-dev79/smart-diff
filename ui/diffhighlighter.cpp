#include "ui/diffhighlighter.h"
#include "theme/thememanager.h"

#include <QTextCursor>
#include <QTextBlock>
#include <QTextCharFormat>

void DiffHighlighter::apply(QPlainTextEdit* leftEditor,
                            QPlainTextEdit* rightEditor,
                            const DiffResult& result)
{
    if (!leftEditor || !rightEditor) return;

    leftEditor->setExtraSelections({});
    rightEditor->setExtraSelections({});

    leftEditor->setExtraSelections(buildLeftSelections(leftEditor, result));
    rightEditor->setExtraSelections(buildRightSelections(rightEditor, result));
}

void DiffHighlighter::clear(QPlainTextEdit* leftEditor,
                            QPlainTextEdit* rightEditor)
{
    if (leftEditor)  leftEditor->setExtraSelections({});
    if (rightEditor) rightEditor->setExtraSelections({});
}

QList<QTextEdit::ExtraSelection> DiffHighlighter::buildLeftSelections(
    QPlainTextEdit* editor,
    const DiffResult& result)
{
    QList<QTextEdit::ExtraSelection> selections;
    const ThemeColors& c = ThemeManager::instance()->colors();

    // Slightly darker background for inline word highlights to ensure contrast
    QColor wordRemovedBg = c.diffRemoved.darker(115);
    int blockIndex = 0;

    for (const DiffHunk& hunk : result.hunks) {
        for (const DiffLine& line : hunk.lines) {
            switch (line.type) {
            case DiffType::Equal:
                blockIndex++;
                break;

            case DiffType::Removed:
                selections.append(makeLineSelection(editor, blockIndex, c.diffRemoved, c.diffRemovedFg));
                blockIndex++;
                break;

            case DiffType::Added:
                // Added lines exist only in the right editor; skip advancing left block index
                break;

            case DiffType::Modified:
                selections.append(makeLineSelection(editor, blockIndex, c.diffChanged, c.text));

                // Inline word-level highlighting
                for (const WordToken& tok : line.leftWordDiff) {
                    if (tok.type == WordDiffType::Removed) {
                        QTextBlock block = editor->document()->findBlockByNumber(blockIndex);
                        if (!block.isValid()) break;

                        QTextEdit::ExtraSelection sel;
                        QTextCursor cur(editor->document());
                        cur.setPosition(block.position() + tok.startCol);
                        cur.setPosition(block.position() + tok.startCol + tok.text.length(), QTextCursor::KeepAnchor);

                        sel.cursor = cur;
                        QTextCharFormat fmt;
                        fmt.setBackground(wordRemovedBg);
                        fmt.setForeground(c.diffRemovedFg);
                        sel.format = fmt;
                        selections.append(sel);
                    }
                }
                blockIndex++;
                break;
            }
        }
    }

    return selections;
}

QList<QTextEdit::ExtraSelection> DiffHighlighter::buildRightSelections(
    QPlainTextEdit* editor,
    const DiffResult& result)
{
    QList<QTextEdit::ExtraSelection> selections;
    const ThemeColors& c = ThemeManager::instance()->colors();

    // Slightly darker background for inline word highlights to ensure contrast
    QColor wordAddedBg = c.diffAdded.darker(115);
    int blockIndex = 0;

    for (const DiffHunk& hunk : result.hunks) {
        for (const DiffLine& line : hunk.lines) {
            switch (line.type) {
            case DiffType::Equal:
                blockIndex++;
                break;

            case DiffType::Added:
                selections.append(makeLineSelection(editor, blockIndex, c.diffAdded, c.diffAddedFg));
                blockIndex++;
                break;

            case DiffType::Removed:
                // Removed lines exist only in the left editor; skip advancing right block index
                break;

            case DiffType::Modified:
                selections.append(makeLineSelection(editor, blockIndex, c.diffChanged, c.text));

                // Inline word-level highlighting
                for (const WordToken& tok : line.rightWordDiff) {
                    if (tok.type == WordDiffType::Added) {
                        QTextBlock block = editor->document()->findBlockByNumber(blockIndex);
                        if (!block.isValid()) break;

                        QTextEdit::ExtraSelection sel;
                        QTextCursor cur(editor->document());
                        cur.setPosition(block.position() + tok.startCol);
                        cur.setPosition(block.position() + tok.startCol + tok.text.length(), QTextCursor::KeepAnchor);

                        sel.cursor = cur;
                        QTextCharFormat fmt;
                        fmt.setBackground(wordAddedBg);
                        fmt.setForeground(c.diffAddedFg);
                        sel.format = fmt;
                        selections.append(sel);
                    }
                }
                blockIndex++;
                break;
            }
        }
    }

    return selections;
}

QTextEdit::ExtraSelection DiffHighlighter::makeLineSelection(
    QPlainTextEdit* editor,
    int             lineIndex,
    const QColor&   bgColor,
    const QColor&   fgColor)
{
    QTextEdit::ExtraSelection sel;
    QTextBlock block = editor->document()->findBlockByNumber(lineIndex);

    if (!block.isValid()) {
        return sel;
    }

    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    sel.cursor = cursor;

    QTextCharFormat fmt;
    fmt.setBackground(bgColor);
    fmt.setForeground(fgColor);

    // FullWidthSelection ensures the highlight extends to the right edge of the editor
    fmt.setProperty(QTextFormat::FullWidthSelection, true);

    sel.format = fmt;
    return sel;
}
