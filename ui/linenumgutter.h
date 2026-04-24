#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextBlock>

class EditorWithGutter : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit EditorWithGutter(QWidget* parent = nullptr)
        : QPlainTextEdit(parent) {}

    QTextBlock firstVisibleBlock() const { return QPlainTextEdit::firstVisibleBlock(); }
    QRectF blockBoundingGeometry(const QTextBlock& b) const { return QPlainTextEdit::blockBoundingGeometry(b); }
    QRectF blockBoundingRect(const QTextBlock& b) const { return QPlainTextEdit::blockBoundingRect(b); }
    QPointF contentOffset() const { return QPlainTextEdit::contentOffset(); }
};

class LineNumGutter : public QWidget
{
    Q_OBJECT

public:
    explicit LineNumGutter(EditorWithGutter* editor, QWidget* parent = nullptr);
    int gutterWidth() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onBlockCountChanged(int newBlockCount);
    void onUpdateRequest(const QRect& rect, int dy);

private:
    void updateWidth();
    EditorWithGutter* m_editor { nullptr };
};
