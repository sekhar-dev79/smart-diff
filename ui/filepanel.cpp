#include "filepanel.h"
#include "theme/thememanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QFontMetrics>
#include <QFrame>

FilePanel::FilePanel(const QString& side, QWidget* parent)
    : QWidget(parent)
{
    buildLayout(side);
}

void FilePanel::buildLayout(const QString& side)
{
    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);

    // ── Panel Header ──────────────────────────────────────────────────
    m_header = new QWidget(this);
    m_header->setProperty("panelHeader", true);
    m_header->setFixedHeight(24); // Tighter height for compact UI

    QHBoxLayout* headerLayout = new QHBoxLayout(m_header);
    headerLayout->setContentsMargins(6, 0, 6, 0);
    headerLayout->setSpacing(6);

    QLabel* sideLabel = new QLabel(side.toUpper(), m_header);
    sideLabel->setProperty("header", true);
    sideLabel->setFixedWidth(40);

    QFrame* divider = new QFrame(m_header);
    divider->setFrameShape(QFrame::VLine);
    divider->setFixedWidth(1);
    divider->setStyleSheet(QString("background-color: %1; border: none;")
                               .arg(ThemeManager::instance()->colors().border.name()));

    m_fileLabel = new QLabel("No file loaded", m_header);
    m_fileLabel->setProperty("filename", true);
    m_fileLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_fileLabel->setMinimumWidth(0);

    m_metaLabel = new QLabel("", m_header);
    m_metaLabel->setProperty("muted", true);
    m_metaLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    headerLayout->addWidget(sideLabel);
    headerLayout->addWidget(divider);
    headerLayout->addWidget(m_fileLabel, 1);
    headerLayout->addWidget(m_metaLabel);

    // ── Editor Row ────────────────────────────────────────────────────
    QWidget* editorRow = new QWidget(this);
    QHBoxLayout* editorLayout = new QHBoxLayout(editorRow);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);

    m_editor = new EditorWithGutter(editorRow);
    m_editor->setReadOnly(true);
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    // Compact monospace font
    QFont monoFont;
    monoFont.setFamilies({
        "JetBrains Mono", "Cascadia Code",
        "Fira Code", "Consolas", "Courier New"
    });
    monoFont.setPointSize(9);
    monoFont.setStyleHint(QFont::TypeWriter);
    m_editor->setFont(monoFont);

    QTextOption opt = m_editor->document()->defaultTextOption();
    opt.setWrapMode(QTextOption::NoWrap);
    m_editor->document()->setDefaultTextOption(opt);

    const int tabStop = QFontMetrics(monoFont).horizontalAdvance(' ') * 4;
    m_editor->setTabStopDistance(tabStop);

    m_gutter = new LineNumGutter(m_editor, editorRow);

    editorLayout->addWidget(m_gutter);
    editorLayout->addWidget(m_editor, 1);

    vbox->addWidget(m_header);
    vbox->addWidget(editorRow, 1);
}

void FilePanel::setFileName(const QString& name)
{
    m_fileLabel->setText(name);
    m_fileLabel->setToolTip(name);
}

void FilePanel::setMetaInfo(const QString& info)
{
    m_metaLabel->setText(info);
}
