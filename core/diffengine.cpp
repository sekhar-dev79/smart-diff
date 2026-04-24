#include "diffengine.h"
#include <vector>
#include <algorithm>
#include <QString>

QVector<DiffEngine::Edit> DiffEngine::computeEdits(const QStringList& a, const QStringList& b)
{
    QVector<Edit> result;
    const int N = a.size();
    const int M = b.size();

    if (N == 0 && M == 0) return result;

    int start = 0;
    while (start < N && start < M && a[start] == b[start]) {
        start++;
    }

    int endA = N - 1;
    int endB = M - 1;
    while (endA >= start && endB >= start && a[endA] == b[endB]) {
        endA--;
        endB--;
    }

    for (int i = 0; i < start; ++i) {
        result.append({Edit::Equal, i, i});
    }

    int subN = endA - start + 1;
    int subM = endB - start + 1;

    if (subN > 0 || subM > 0) {
        if (subN == 0) {
            for (int j = 0; j < subM; ++j) {
                result.append({Edit::Insert, -1, start + j});
            }
        } else if (subM == 0) {
            for (int i = 0; i < subN; ++i) {
                result.append({Edit::Delete, start + i, -1});
            }
        } else {
            const int MAX_D = 3000;
            int limitD = std::min(subN + subM, MAX_D);
            int offset = limitD;

            std::vector<int> V(2 * limitD + 1, -1);
            std::vector<std::vector<int>> trace;

            V[offset + 1] = 0;
            int finalD = -1;

            for (int d = 0; d <= limitD; ++d) {
                bool reachedEnd = false;

                for (int k = -d; k <= d; k += 2) {
                    bool down = (k == -d || (k != d && V[offset + k - 1] < V[offset + k + 1]));
                    int prevK = down ? k + 1 : k - 1;
                    int x = down ? V[offset + prevK] : V[offset + prevK] + 1;
                    int y = x - k;

                    while (x < subN && y < subM && a[start + x] == b[start + y]) {
                        x++;
                        y++;
                    }
                    V[offset + k] = x;

                    if (x >= subN && y >= subM) {
                        reachedEnd = true;
                        finalD = d;
                        break;
                    }
                }

                // CRITICAL FIX: Trace must be pushed AFTER the states are computed
                trace.push_back(V);

                if (reachedEnd) break;
            }

            if (finalD == -1) {
                for (int i = 0; i < subN; ++i) result.append({Edit::Delete, start + i, -1});
                for (int j = 0; j < subM; ++j) result.append({Edit::Insert, -1, start + j});
            } else {
                QVector<Edit> subEdits;
                int x = subN;
                int y = subM;

                for (int d = finalD; d > 0; --d) {
                    const auto& v = trace[d - 1];
                    int k = x - y;
                    bool down = (k == -d || (k != d && v[offset + k - 1] < v[offset + k + 1]));
                    int prevK = down ? k + 1 : k - 1;
                    int prevX = v[offset + prevK];
                    int prevY = prevX - prevK;

                    while (x > prevX && y > prevY) {
                        subEdits.append({Edit::Equal, start + x - 1, start + y - 1});
                        x--; y--;
                    }

                    if (x > prevX) {
                        subEdits.append({Edit::Delete, start + prevX, -1});
                    } else {
                        subEdits.append({Edit::Insert, -1, start + prevY});
                    }
                    x = prevX;
                    y = prevY;
                }

                while (x > 0 && y > 0) {
                    subEdits.append({Edit::Equal, start + x - 1, start + y - 1});
                    x--; y--;
                }

                std::reverse(subEdits.begin(), subEdits.end());
                result.append(subEdits);
            }
        }
    }

    for (int i = 0; i < N - 1 - endA; ++i) {
        result.append({Edit::Equal, endA + 1 + i, endB + 1 + i});
    }

    return result;
}

DiffResult DiffEngine::diff(const QStringList& leftLines,
                            const QStringList& rightLines,
                            bool               ignoreWhitespace)
{
    QStringList left = leftLines;
    QStringList right = rightLines;

    if (ignoreWhitespace) {
        for (auto& l : left)  l = l.trimmed();
        for (auto& r : right) r = r.trimmed();
    }

    if (left.isEmpty() && right.isEmpty())
        return DiffResult{};

    QVector<Edit> edits = computeEdits(left, right);

    QVector<DiffLine> lines;
    lines.reserve(edits.size());

    int leftNo = 1, rightNo = 1;

    for (const Edit& e : std::as_const(edits)) {
        DiffLine dl;

        switch (e.type) {
        case Edit::Equal:
            if (e.oldIdx < 0 || e.oldIdx >= leftLines.size())  continue;
            if (e.newIdx < 0 || e.newIdx >= rightLines.size()) continue;
            dl.type        = DiffType::Equal;
            dl.leftText    = ignoreWhitespace ? leftLines[e.oldIdx]  : left[e.oldIdx];
            dl.rightText   = ignoreWhitespace ? rightLines[e.newIdx] : right[e.newIdx];
            dl.leftLineNo  = leftNo++;
            dl.rightLineNo = rightNo++;
            break;

        case Edit::Delete:
            if (e.oldIdx < 0 || e.oldIdx >= leftLines.size()) continue;
            dl.type       = DiffType::Removed;
            dl.leftText   = ignoreWhitespace ? leftLines[e.oldIdx] : left[e.oldIdx];
            dl.leftLineNo = leftNo++;
            break;

        case Edit::Insert:
            if (e.newIdx < 0 || e.newIdx >= rightLines.size()) continue;
            dl.type        = DiffType::Added;
            dl.rightText   = ignoreWhitespace ? rightLines[e.newIdx] : right[e.newIdx];
            dl.rightLineNo = rightNo++;
            break;
        }
        lines.append(dl);
    }

    detectModified(lines);
    return buildResult(groupIntoHunks(lines));
}

void DiffEngine::wordDiff(const QString&      leftLine,
                          const QString&      rightLine,
                          QVector<WordToken>& leftTokens,
                          QVector<WordToken>& rightTokens)
{
    const QStringList lw = tokenize(leftLine);
    const QStringList rw = tokenize(rightLine);

    if (lw.isEmpty() && rw.isEmpty()) return;

    QVector<Edit> edits = computeEdits(lw, rw);

    int col = 0;
    for (const Edit& e : std::as_const(edits)) {
        if (e.type == Edit::Equal || e.type == Edit::Delete) {
            if (e.oldIdx < 0 || e.oldIdx >= lw.size()) continue;
            WordToken tok;
            tok.type     = (e.type == Edit::Delete) ? WordDiffType::Removed : WordDiffType::Equal;
            tok.text     = lw[e.oldIdx];
            tok.startCol = col;
            col += tok.text.length();
            leftTokens.append(tok);
        }
    }

    col = 0;
    for (const Edit& e : std::as_const(edits)) {
        if (e.type == Edit::Equal || e.type == Edit::Insert) {
            if (e.newIdx < 0 || e.newIdx >= rw.size()) continue;
            WordToken tok;
            tok.type     = (e.type == Edit::Insert) ? WordDiffType::Added : WordDiffType::Equal;
            tok.text     = rw[e.newIdx];
            tok.startCol = col;
            col += tok.text.length();
            rightTokens.append(tok);
        }
    }
}

QStringList DiffEngine::tokenize(const QString& line)
{
    QStringList tokens;
    if (line.isEmpty()) return tokens;

    QString current;
    bool inWord = !line[0].isSpace();

    for (const QChar& ch : std::as_const(line)) {
        const bool isWord = !ch.isSpace();
        if (isWord == inWord) {
            current += ch;
        } else {
            if (!current.isEmpty()) tokens.append(current);
            current = ch;
            inWord  = isWord;
        }
    }
    if (!current.isEmpty()) tokens.append(current);
    return tokens;
}

void DiffEngine::detectModified(QVector<DiffLine>& lines)
{
    QVector<DiffLine> result;
    result.reserve(lines.size());

    int i = 0;
    const int total = lines.size();

    while (i < total) {
        if (i + 1 < total &&
            lines[i].type     == DiffType::Removed &&
            lines[i + 1].type == DiffType::Added)
        {
            DiffLine mod;
            mod.type        = DiffType::Modified;
            mod.leftText    = lines[i].leftText;
            mod.rightText   = lines[i + 1].rightText;
            mod.leftLineNo  = lines[i].leftLineNo;
            mod.rightLineNo = lines[i + 1].rightLineNo;

            wordDiff(mod.leftText, mod.rightText,
                     mod.leftWordDiff, mod.rightWordDiff);

            result.append(std::move(mod));
            i += 2;
        } else {
            result.append(lines[i]);
            ++i;
        }
    }

    lines = std::move(result);
}

QVector<DiffHunk> DiffEngine::groupIntoHunks(const QVector<DiffLine>& lines)
{
    QVector<DiffHunk> hunks;
    if (lines.isEmpty()) return hunks;

    DiffHunk current;
    current.type = lines.first().type;

    for (const DiffLine& dl : std::as_const(lines)) {
        if (dl.type == current.type) {
            current.lines.append(dl);
        } else {
            hunks.append(std::move(current));
            current      = DiffHunk{};
            current.type = dl.type;
            current.lines.append(dl);
        }
    }
    hunks.append(std::move(current));
    return hunks;
}

DiffResult DiffEngine::buildResult(const QVector<DiffHunk>& hunks)
{
    DiffResult res;
    res.hunks = hunks;

    for (const DiffHunk& h : std::as_const(hunks)) {
        switch (h.type) {
        case DiffType::Added:    res.addedCount    += h.lineCount(); break;
        case DiffType::Removed:  res.removedCount  += h.lineCount(); break;
        case DiffType::Equal:    res.equalCount    += h.lineCount(); break;
        case DiffType::Modified: res.modifiedCount += h.lineCount(); break;
        }
    }
    return res;
}
