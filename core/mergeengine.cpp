#include "core/mergeengine.h"
#include <QTextCursor>
#include <QTextDocument>

QStringList MergeEngine::currentLines(QPlainTextEdit* editor)
{
    if (!editor) return {};
    return editor->toPlainText().split('\n');
}

static QStringList buildMergedLines(const DiffResult& result)
{
    QStringList merged;

    for (const DiffHunk& hunk : result.hunks) {
        switch (hunk.type) {
        case DiffType::Equal:
        case DiffType::Modified:
        case DiffType::Removed:
            for (const DiffLine& dl : hunk.lines) {
                merged.append(dl.leftText);
            }
            break;
        case DiffType::Added:
            // Discard right-only lines during an accept operation
            break;
        }
    }

    return merged;
}

void MergeEngine::acceptAll(QPlainTextEdit* rightEditor,
                            const DiffResult& result)
{
    if (!rightEditor || result.hunks.isEmpty()) return;
    rightEditor->setExtraSelections({});

    const QStringList mergedLines = buildMergedLines(result);
    const QString mergedText = mergedLines.join('\n');

    QTextCursor cursor(rightEditor->document());
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);

    cursor.removeSelectedText();

    cursor.insertText(mergedText);
    cursor.endEditBlock();
}

bool MergeEngine::acceptHunk(QPlainTextEdit* rightEditor,
                             const DiffResult& result,
                             int               hunkIndex)
{
    if (!rightEditor) return false;
    if (hunkIndex < 0 || hunkIndex >= result.hunks.size()) return false;

    QStringList merged;

    for (int i = 0; i < result.hunks.size(); ++i) {
        const DiffHunk& hunk = result.hunks[i];

        if (i != hunkIndex) {
            for (const DiffLine& dl : hunk.lines) {
                if (dl.type != DiffType::Removed) {
                    merged.append(dl.rightText);
                }
            }
        } else {
            switch (hunk.type) {
            case DiffType::Equal:
            case DiffType::Removed:
            case DiffType::Modified:
                for (const DiffLine& dl : hunk.lines) {
                    merged.append(dl.leftText);
                }
                break;
            case DiffType::Added:
                break;
            }
        }
    }

    const QString mergedText = merged.join('\n');

    QTextCursor cursor(rightEditor->document());
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);

    cursor.removeSelectedText();

    cursor.insertText(mergedText);
    cursor.endEditBlock();

    return true;
}
