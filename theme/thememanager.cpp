#include "thememanager.h"
#include <QApplication>

ThemeManager* ThemeManager::s_instance = nullptr;

ThemeManager* ThemeManager::instance()
{
    if (!s_instance)
        s_instance = new ThemeManager(qApp);
    return s_instance;
}

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
{
    m_colors = lightPalette();
}

void ThemeManager::applyTheme(Theme theme)
{
    m_currentTheme = theme;
    m_colors = (theme == Theme::Dark) ? darkPalette() : lightPalette();
    qApp->setStyleSheet(buildStyleSheet(m_colors));
    emit themeChanged(theme);
}

void ThemeManager::toggleTheme()
{
    applyTheme(isDark() ? Theme::Light : Theme::Dark);
}

ThemeColors ThemeManager::lightPalette()
{
    ThemeColors c;

    // Classic Neutral Light
    c.bg          = QColor("#F0F0F0");
    c.surface     = QColor("#FFFFFF");
    c.surfaceAlt  = QColor("#E4E4E4");
    c.border      = QColor("#D4D4D4");

    c.text        = QColor("#1E1E1E");
    c.textMuted   = QColor("#666666");
    c.textFaint   = QColor("#999999");

    c.diffAdded   = QColor("#E6FFED");
    c.diffRemoved = QColor("#FFEBE9");
    c.diffChanged = QColor("#FFF8C5");
    c.diffAddedFg = QColor("#1A7F37");
    c.diffRemovedFg = QColor("#CF222E");

    c.accent      = QColor("#0078D4");
    c.accentHover = QColor("#106EBE");
    c.accentText  = QColor("#FFFFFF");

    c.iconColor   = QColor("#424242");
    c.iconMuted   = QColor("#8A8A8A");

    c.lineNumBg        = QColor("#F0F0F0");
    c.lineNumFg        = QColor("#999999");
    c.lineNumCurrentFg = QColor("#1E1E1E");

    return c;
}

ThemeColors ThemeManager::darkPalette()
{
    ThemeColors c;

    // Classic Neutral Dark (IDE standard)
    c.bg          = QColor("#1E1E1E");
    c.surface     = QColor("#252526");
    c.surfaceAlt  = QColor("#333333");
    c.border      = QColor("#454545");

    c.text        = QColor("#D4D4D4");
    c.textMuted   = QColor("#858585");
    c.textFaint   = QColor("#555555");

    c.diffAdded   = QColor("#204E26");
    c.diffRemoved = QColor("#5A1D1D");
    c.diffChanged = QColor("#5C4B17");
    c.diffAddedFg = QColor("#46C05E");
    c.diffRemovedFg = QColor("#FF6B6B");

    c.accent      = QColor("#00E5FF");
    c.accentHover = QColor("#00B8D4");
    c.accentText  = QColor("#1E1E1E");

    c.iconColor   = QColor("#CCCCCC");
    c.iconMuted   = QColor("#707070");

    c.lineNumBg        = QColor("#1E1E1E");
    c.lineNumFg        = QColor("#707070");
    c.lineNumCurrentFg = QColor("#D4D4D4");

    return c;
}

// ── QSS Template ─────────────────────────────────────────────────────────────

QString ThemeManager::buildStyleSheet(const ThemeColors& c) const
{
    QString style = R"(
QWidget {
    background-color: %1;
    color: %5;
    font-family: "Inter", "Segoe UI", system-ui, sans-serif;
    font-size: 11px;
    border: none;
    outline: none;
    selection-background-color: %12;
    selection-color: %5;
}

QMainWindow {
    background-color: %1;
}

QMainWindow::separator {
    background-color: %4;
    width: 1px;
    height: 1px;
}

QToolBar {
    background-color: %3;
    border-bottom: 1px solid %4;
    spacing: 0px;
    padding: 1px 4px;
    min-height: 28px;
    max-height: 28px;
}

QToolBar::separator {
    background-color: %4;
    width: 1px;
    margin: 3px 4px;
}

QToolButton {
    background-color: transparent;
    color: %5;
    border: 1px solid transparent;
    border-radius: 2px;
    padding: 2px 6px;
    font-size: 11px;
    min-height: 22px;
    min-width: 22px;
}

QToolButton:hover {
    background-color: %4;
}

QToolButton:pressed {
    background-color: %3;
}

QToolButton:disabled {
    color: %7;
}

QToolButton:checked {
    background-color: %4;
    border-color: %8;
    color: %8;
}

QPushButton {
    background-color: %8;
    color: %9;
    border: none;
    border-radius: 2px;
    padding: 2px 10px;
    font-size: 11px;
    font-weight: 500;
    min-height: 22px;
}

QPushButton:hover {
    background-color: %10;
}

QPushButton:pressed {
    background-color: %8;
    color: %5;
}

QPushButton:disabled {
    background-color: %4;
    color: %7;
}

QPushButton[flat="true"],
QPushButton[secondary="true"] {
    background-color: transparent;
    color: %5;
    border: 1px solid %4;
}

QPushButton[flat="true"]:hover,
QPushButton[secondary="true"]:hover {
    background-color: %4;
    border-color: %6;
}

QLabel {
    background-color: transparent;
    color: %5;
    padding: 0;
}

QLabel[muted="true"] {
    color: %6;
    font-size: 10px;
}

QLabel[header="true"] {
    color: %5;
    font-size: 10px;
    font-weight: 600;
    letter-spacing: 0.5px;
    text-transform: uppercase;
    padding: 0 2px;
}

QLabel[filename="true"] {
    color: %5;
    font-size: 11px;
    font-weight: 600;
    padding: 0 2px;
}

QPlainTextEdit {
    background-color: %2;
    color: %5;
    selection-background-color: %12;
    selection-color: %5;
    border: none;
    border-radius: 0;
    padding: 0;
    font-family: "JetBrains Mono", "Cascadia Code", "Fira Code",
                 "Consolas", "Courier New", monospace;
    font-size: 12px;
    line-height: 1.4;
}

QPlainTextEdit:focus {
    border: none;
    outline: none;
}

QScrollBar:vertical {
    background-color: %2;
    width: 10px;
    margin: 0;
}

QScrollBar::handle:vertical {
    background-color: %4;
    border-radius: 0px;
    min-height: 20px;
    margin: 1px;
}

QScrollBar::handle:vertical:hover {
    background-color: %6;
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical,
QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    height: 0;
    background: none;
}

QScrollBar:horizontal {
    background-color: %2;
    height: 10px;
    margin: 0;
}

QScrollBar::handle:horizontal {
    background-color: %4;
    border-radius: 0px;
    min-width: 20px;
    margin: 1px;
}

QScrollBar::handle:horizontal:hover {
    background-color: %6;
}

QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal,
QScrollBar::add-page:horizontal,
QScrollBar::sub-page:horizontal {
    width: 0;
    background: none;
}

QSplitter::handle {
    background-color: %4;
}

QSplitter::handle:horizontal {
    width: 1px;
}

QSplitter::handle:vertical {
    height: 1px;
}

QStatusBar {
    background-color: %3;
    color: %6;
    border-top: 1px solid %4;
    font-size: 10px;
    min-height: 20px;
    max-height: 20px;
    padding: 0 6px;
}

QStatusBar::item {
    border: none;
}

QMenuBar {
    background-color: %3;
    color: %5;
    border-bottom: 1px solid %4;
    font-size: 11px;
    padding: 0px 2px;
    min-height: 24px;
    max-height: 24px;
}

QMenuBar::item {
    background-color: transparent;
    padding: 2px 6px;
    border-radius: 2px;
}

QMenuBar::item:selected,
QMenuBar::item:pressed {
    background-color: %4;
}

QMenu {
    background-color: %2;
    color: %5;
    border: 1px solid %4;
    border-radius: 2px;
    padding: 2px;
    font-size: 11px;
}

QMenu::item {
    padding: 4px 18px 4px 8px;
    border-radius: 2px;
}

QMenu::item:selected {
    background-color: %4;
}

QMenu::separator {
    height: 1px;
    background-color: %4;
    margin: 2px 4px;
}

QToolTip {
    background-color: %2;
    color: %5;
    border: 1px solid %4;
    border-radius: 2px;
    padding: 3px 6px;
    font-size: 10px;
}

QMessageBox {
    background-color: %1;
    font-size: 11px;
}

QMessageBox QLabel {
    color: %5;
    font-size: 11px;
}

QWidget[panelHeader="true"] {
    background-color: %3;
    border-bottom: 1px solid %4;
    min-height: 24px;
    max-height: 24px;
}

QWidget[lineGutter="true"] {
    background-color: %11;
    border-right: 1px solid %4;
}
    )";

    // By grouping into batches of 9 and 3, Qt replaces them safely
    // without shifting or misinterpreting the numbers.
    return style.arg(
                    c.bg.name(),           // %1
                    c.surface.name(),      // %2 (Editor BG)
                    c.surfaceAlt.name(),   // %3
                    c.border.name(),       // %4
                    c.text.name(),         // %5
                    c.textMuted.name(),    // %6
                    c.textFaint.name(),    // %7
                    c.accent.name(),       // %8
                    c.accentText.name()    // %9
                    ).arg(
            c.accentHover.name(),  // %10
            c.lineNumBg.name(),    // %11
            c.diffChanged.name()   // %12
            );
}
