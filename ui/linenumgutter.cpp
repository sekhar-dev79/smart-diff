#include "linenumgutter.h"
#include "theme/thememanager.h"

#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>

LineNumGutter::LineNumGutter(EditorWithGutter* editor, QWidget* parent)
    : QWidget(parent)
    , m_editor(editor)
{
    setProperty("lineGutter", true);

    connect(m_editor, &QPlainTextEdit::blockCountChanged,
            this, &LineNumGutter::onBlockCountChanged);

    connect(m_editor, &QPlainTextEdit::updateRequest,
            this, &LineNumGutter::onUpdateRequest);

    updateWidth();
}

int LineNumGutter::gutterWidth() const
{
    const int lineCount = static_cast<int>(m_editor->blockCount());
    const int digits = std::max(3, static_cast<int>(QString::number(lineCount).length()));

    // Sync with editor's font to ensure exact width calculation
    QFontMetrics fm(m_editor->font());
    const int charWidth = fm.horizontalAdvance(QLatin1Char('9'));

    // Ultra-compact padding: 4px left + 4px right
    return digits * charWidth + 8;
}

void LineNumGutter::updateWidth()
{
    setFixedWidth(gutterWidth());
}

void LineNumGutter::onBlockCountChanged(int /*newBlockCount*/)
{
    updateWidth();
}

void LineNumGutter::onUpdateRequest(const QRect& rect, int dy)
{
    if (dy)
        scroll(0, dy);
    else
        update(0, rect.y(), width(), rect.height());
}

void LineNumGutter::paintEvent(QPaintEvent* event)
{
    const ThemeColors& c = ThemeManager::instance()->colors();

    QPainter painter(this);
    painter.fillRect(event->rect(), c.lineNumBg);

    // Subtle right border
    painter.setPen(QPen(c.border, 1));
    painter.drawLine(width() - 1, event->rect().top(),
                     width() - 1, event->rect().bottom());

    // Guarantee font matches the editor perfectly to align baselines
    painter.setFont(m_editor->font());

    QTextBlock block = m_editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();

    const qreal offsetY = m_editor->contentOffset().y();
    qreal top = m_editor->blockBoundingGeometry(block).translated(0, offsetY).top();
    qreal bottom = top + m_editor->blockBoundingRect(block).height();

    // Tighter right padding to match the new 8px total width padding
    const int rightPad = 4;

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            const bool isCurrent = (blockNumber == m_editor->textCursor().blockNumber());

            painter.setPen(isCurrent ? c.lineNumCurrentFg : c.lineNumFg);

            painter.drawText(
                0,
                static_cast<int>(top),
                width() - rightPad,
                static_cast<int>(m_editor->blockBoundingRect(block).height()),
                Qt::AlignRight | Qt::AlignVCenter,
                number
                );
        }

        block = block.next();
        top = bottom;
        bottom = top + m_editor->blockBoundingRect(block).height();
        ++blockNumber;
    }
}
