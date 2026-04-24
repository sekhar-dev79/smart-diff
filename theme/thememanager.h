#pragma once

#include <QObject>
#include <QString>
#include <QColor>

struct ThemeColors {
    QColor bg;
    QColor surface;
    QColor surfaceAlt;
    QColor border;

    QColor text;
    QColor textMuted;
    QColor textFaint;

    QColor diffAdded;
    QColor diffRemoved;
    QColor diffChanged;
    QColor diffAddedFg;
    QColor diffRemovedFg;

    QColor accent;
    QColor accentHover;
    QColor accentText;

    QColor iconColor;
    QColor iconMuted;

    QColor lineNumBg;
    QColor lineNumFg;
    QColor lineNumCurrentFg;
};

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    static ThemeManager* instance();

    enum class Theme { Light, Dark };

    void applyTheme(Theme theme);
    void toggleTheme();

    Theme currentTheme() const { return m_currentTheme; }
    bool isDark() const { return m_currentTheme == Theme::Dark; }
    const ThemeColors& colors() const { return m_colors; }

signals:
    void themeChanged(Theme newTheme);

private:
    explicit ThemeManager(QObject* parent = nullptr);

    QString buildStyleSheet(const ThemeColors& c) const;
    static ThemeColors lightPalette();
    static ThemeColors darkPalette();

    Theme m_currentTheme { Theme::Light };
    ThemeColors m_colors;

    static ThemeManager* s_instance;
};
