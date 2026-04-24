#include "scrollsynchronizer.h"
#include <QScrollBar>

ScrollSynchronizer::ScrollSynchronizer(QPlainTextEdit* leftEditor,
                                       QPlainTextEdit* rightEditor,
                                       QObject* parent)
    : QObject(parent)
    , m_leftEditor(leftEditor)
    , m_rightEditor(rightEditor)
{
}

void ScrollSynchronizer::setEnabled(bool enabled)
{
    if (m_enabled == enabled) return;
    m_enabled = enabled;

    if (enabled)
        connectScrollBars();
    else
        disconnectScrollBars();
}

void ScrollSynchronizer::connectScrollBars()
{
    if (!m_leftEditor || !m_rightEditor) return;

    connect(m_leftEditor->verticalScrollBar(),   &QScrollBar::valueChanged,
            this, &ScrollSynchronizer::onLeftVerticalScroll);
    connect(m_rightEditor->verticalScrollBar(),  &QScrollBar::valueChanged,
            this, &ScrollSynchronizer::onRightVerticalScroll);
    connect(m_leftEditor->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &ScrollSynchronizer::onLeftHorizontalScroll);
    connect(m_rightEditor->horizontalScrollBar(),&QScrollBar::valueChanged,
            this, &ScrollSynchronizer::onRightHorizontalScroll);
}

void ScrollSynchronizer::disconnectScrollBars()
{
    if (!m_leftEditor || !m_rightEditor) return;

    disconnect(m_leftEditor->verticalScrollBar(),   &QScrollBar::valueChanged,
               this, &ScrollSynchronizer::onLeftVerticalScroll);
    disconnect(m_rightEditor->verticalScrollBar(),  &QScrollBar::valueChanged,
               this, &ScrollSynchronizer::onRightVerticalScroll);
    disconnect(m_leftEditor->horizontalScrollBar(), &QScrollBar::valueChanged,
               this, &ScrollSynchronizer::onLeftHorizontalScroll);
    disconnect(m_rightEditor->horizontalScrollBar(),&QScrollBar::valueChanged,
               this, &ScrollSynchronizer::onRightHorizontalScroll);
}

// ── Scroll Slots ─────────────────────────────────────────────────────────────

void ScrollSynchronizer::onLeftVerticalScroll(int value)
{
    if (m_syncing) return;
    m_syncing = true;
    m_rightEditor->verticalScrollBar()->setValue(value);
    m_syncing = false;
}

void ScrollSynchronizer::onRightVerticalScroll(int value)
{
    if (m_syncing) return;
    m_syncing = true;
    m_leftEditor->verticalScrollBar()->setValue(value);
    m_syncing = false;
}

void ScrollSynchronizer::onLeftHorizontalScroll(int value)
{
    if (m_syncing) return;
    m_syncing = true;
    m_rightEditor->horizontalScrollBar()->setValue(value);
    m_syncing = false;
}

void ScrollSynchronizer::onRightHorizontalScroll(int value)
{
    if (m_syncing) return;
    m_syncing = true;
    m_leftEditor->horizontalScrollBar()->setValue(value);
    m_syncing = false;
}

// ── State Management ─────────────────────────────────────────────────────────

void ScrollSynchronizer::reset()
{
    const bool wasEnabled = m_enabled;
    if (wasEnabled) setEnabled(false);

    m_leftEditor->verticalScrollBar()->setValue(0);
    m_leftEditor->horizontalScrollBar()->setValue(0);
    m_rightEditor->verticalScrollBar()->setValue(0);
    m_rightEditor->horizontalScrollBar()->setValue(0);

    if (wasEnabled) setEnabled(true);
}

void ScrollSynchronizer::scrollToLine(int blockNumber)
{
    const bool wasEnabled = m_enabled;
    if (wasEnabled) setEnabled(false);

    m_leftEditor->verticalScrollBar()->setValue(blockNumber);
    m_rightEditor->verticalScrollBar()->setValue(blockNumber);

    if (wasEnabled) setEnabled(true);
}
