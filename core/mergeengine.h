#pragma once

#include "models/diffmodel.h"
#include <QPlainTextEdit>
#include <QStringList>

class MergeEngine
{
public:
    // Replaces the full right document with a freshly computed merged result
    static void acceptAll(QPlainTextEdit* rightEditor,
                          const DiffResult& result);

    // Applies a single hunk's changes and recompiles the document
    static bool acceptHunk(QPlainTextEdit* rightEditor,
                           const DiffResult& result,
                           int               hunkIndex);

    static QStringList currentLines(QPlainTextEdit* editor);

private:
    MergeEngine() = delete;
};
