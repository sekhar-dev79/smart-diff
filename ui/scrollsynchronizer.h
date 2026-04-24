#pragma once

#include <QObject>
#include <QPlainTextEdit>

// Links the scroll bars of two QPlainTextEdit widgets.
// Uses a re-entrancy guard to prevent infinite feedback loops during sync.
class ScrollSynchronizer : public QObject
{
    Q_OBJECT

public:
    explicit ScrollSynchronizer(QPlainTextEdit* leftEditor,
                                QPlainTextEdit* rightEditor,
                                QObject* parent = nullptr);

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    void reset();
    void scrollToLine(int blockNumber);

private slots:
    void onLeftVerticalScroll(int value);
    void onRightVerticalScroll(int value);
    void onLeftHorizontalScroll(int value);
    void onRightHorizontalScroll(int value);

private:
    void connectScrollBars();
    void disconnectScrollBars();

    QPlainTextEdit* m_leftEditor  { nullptr };
    QPlainTextEdit* m_rightEditor { nullptr };

    bool m_syncing { false }; // Re-entrancy guard
    bool m_enabled { false };
};
