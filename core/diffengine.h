#pragma once

#include "models/diffmodel.h"
#include <QStringList>
#include <QVector>

class DiffEngine
{
public:
    static DiffResult diff(const QStringList& leftLines,
                           const QStringList& rightLines,
                           bool ignoreWhitespace = false);

    static void wordDiff(const QString&      leftLine,
                         const QString&      rightLine,
                         QVector<WordToken>& leftTokens,
                         QVector<WordToken>& rightTokens);

private:
    struct Edit {
        enum Type { Equal, Insert, Delete } type;
        int oldIdx { -1 };
        int newIdx { -1 };
    };

    static QVector<Edit> computeEdits(const QStringList& a,
                                      const QStringList& b);

    static QStringList       tokenize(const QString& line);
    static void              detectModified(QVector<DiffLine>& lines);
    static QVector<DiffHunk> groupIntoHunks(const QVector<DiffLine>& lines);
    static DiffResult        buildResult(const QVector<DiffHunk>& hunks);

    DiffEngine() = delete;
};
