#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

enum class DiffType {
    Equal,
    Added,
    Removed,
    Modified
};

enum class WordDiffType {
    Equal,
    Added,
    Removed
};

struct WordToken {
    WordDiffType type { WordDiffType::Equal };
    QString text;
    int startCol { 0 };
};

struct DiffLine {
    DiffType type { DiffType::Equal };
    QString leftText;
    QString rightText;
    int leftLineNo { -1 };
    int rightLineNo { -1 };

    QVector<WordToken> leftWordDiff;
    QVector<WordToken> rightWordDiff;
};

struct DiffHunk {
    DiffType type;
    QVector<DiffLine> lines;
    int lineCount() const { return lines.size(); }
};

struct DiffResult {
    QVector<DiffHunk> hunks;
    int addedCount { 0 };
    int removedCount { 0 };
    int equalCount { 0 };
    int modifiedCount { 0 };

    bool isIdentical() const { return addedCount == 0 && removedCount == 0 && modifiedCount == 0; }
    int totalLines() const { return addedCount + removedCount + equalCount + modifiedCount; }
};
