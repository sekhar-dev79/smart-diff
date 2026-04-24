#include "mainwindow.h"
#include "thememanager.h"
#include "iconmanager.h"
#include "iconnames.h"
#include "fileloader.h"
#include "diffengine.h"
#include "diffmodel.h"
#include "diffhighlighter.h"
#include "scrollsynchronizer.h"
#include "mergeengine.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileInfo>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("SmartDiff");
    resize(1280, 720);
    setMinimumSize(800, 500);

    buildToolbar();
    buildCentralWidget();

    connect(m_rightPanel->editor()->document(), &QTextDocument::undoAvailable,
            m_undoAction, &QAction::setEnabled);
    connect(m_rightPanel->editor()->document(), &QTextDocument::modificationChanged,
            m_saveRightAction, &QAction::setEnabled);

    buildStatusBar();

    connect(ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &MainWindow::onThemeChanged);
}

void MainWindow::buildToolbar()
{
    QToolBar* toolbar = new QToolBar("Main Toolbar", this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setIconSize(QSize(16, 16));
    addToolBar(Qt::TopToolBarArea, toolbar);

    m_actOpenLeft = new QAction("Open Left", this);
    m_actOpenLeft->setShortcut(QKeySequence("Ctrl+L"));
    connect(m_actOpenLeft, &QAction::triggered, this, &MainWindow::onOpenLeftFile);
    toolbar->addAction(m_actOpenLeft);

    m_actOpenRight = new QAction("Open Right", this);
    m_actOpenRight->setShortcut(QKeySequence("Ctrl+R"));
    connect(m_actOpenRight, &QAction::triggered, this, &MainWindow::onOpenRightFile);
    toolbar->addAction(m_actOpenRight);

    toolbar->addSeparator();

    m_actCompare = new QAction("Compare", this);
    m_actCompare->setShortcut(QKeySequence("Ctrl+D"));
    m_actCompare->setEnabled(false);
    connect(m_actCompare, &QAction::triggered, this, &MainWindow::onCompare);
    toolbar->addAction(m_actCompare);

    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    m_actTheme = new QAction("Theme", this);
    m_actTheme->setShortcut(QKeySequence("Ctrl+T"));
    connect(m_actTheme, &QAction::triggered, this, &MainWindow::onToggleTheme);
    toolbar->addAction(m_actTheme);

    toolbar->addSeparator();

    m_acceptAllAction = toolbar->addAction(
        IconManager::instance()->icon(Icons::Merge, QSize(16, 16)), "Accept All");
    m_acceptAllAction->setEnabled(false);
    connect(m_acceptAllAction, &QAction::triggered, this, &MainWindow::onAcceptAll);

    toolbar->addSeparator();

    m_undoAction = toolbar->addAction(
        IconManager::instance()->icon(Icons::Undo, QSize(16, 16)), "Undo");
    m_undoAction->setEnabled(false);
    m_undoAction->setShortcut(QKeySequence::Undo);
    connect(m_undoAction, &QAction::triggered, [this]() {
        DiffHighlighter::clear(m_leftPanel->editor(), m_rightPanel->editor());
        m_rightPanel->editor()->document()->undo();
        refreshAfterMerge();
    });

    m_saveRightAction = toolbar->addAction(
        IconManager::instance()->icon(Icons::Save, QSize(16, 16)), "Save Right");
    m_saveRightAction->setEnabled(false);
    connect(m_saveRightAction, &QAction::triggered, this, &MainWindow::onSaveRight);

    refreshIcons();
}

void MainWindow::buildCentralWidget()
{
    QWidget* central = new QWidget(this);
    QVBoxLayout* vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);

    m_splitter = new QSplitter(Qt::Horizontal, central);
    m_splitter->setHandleWidth(1);
    m_splitter->setChildrenCollapsible(false);

    m_leftPanel  = new FilePanel("Left", m_splitter);
    m_rightPanel = new FilePanel("Right", m_splitter);

    m_splitter->addWidget(m_leftPanel);
    m_splitter->addWidget(m_rightPanel);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);

    m_scrollSync = new ScrollSynchronizer(m_leftPanel->editor(), m_rightPanel->editor(), this);

    vbox->addWidget(m_splitter);
    setCentralWidget(central);
}

void MainWindow::buildStatusBar()
{
    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::onOpenLeftFile()
{
    const FileResult result = FileLoader::openWithDialog(this, "Open Original", m_lastDirectory);
    applyFileResult(m_leftPanel, result, true);
}

void MainWindow::onOpenRightFile()
{
    const FileResult result = FileLoader::openWithDialog(this, "Open Modified", m_lastDirectory);
    applyFileResult(m_rightPanel, result, false);
}

void MainWindow::applyFileResult(FilePanel* panel, const FileResult& result, bool isLeft)
{
    if (!result.success) {
        if (!result.errorMessage.isEmpty()) QMessageBox::warning(this, "File Error", result.errorMessage);
        return;
    }

    m_scrollSync->setEnabled(false);
    panel->setFileName(result.fileName);
    panel->setMetaInfo(QString("%1 lines | %2 | %3")
                           .arg(result.lines.count()).arg(result.encoding)
                           .arg(FileLoader::formatSize(result.fileSize)));

    panel->editor()->setPlainText(result.fullText);
    if (isLeft) m_leftFile = result;
    else        m_rightFile = result;

    m_lastDirectory = QFileInfo(result.filePath).absolutePath();
    m_scrollSync->reset();
    DiffHighlighter::clear(m_leftPanel->editor(), m_rightPanel->editor());
    m_diffResult = DiffResult{};
    updateCompareButton();
}

void MainWindow::updateCompareButton()
{
    m_actCompare->setEnabled(m_leftFile.success && m_rightFile.success);
}

void MainWindow::showDiffSummary(const DiffResult& result)
{
    if (result.isIdentical()) {
        m_statusLabel->setText("Files are identical");
        return;
    }
    m_statusLabel->setText(QString("+%1 added | -%2 removed | ~%3 modified | =%4 unchanged")
                               .arg(result.addedCount).arg(result.removedCount)
                               .arg(result.modifiedCount).arg(result.equalCount));
}

void MainWindow::onCompare()
{
    if (!m_leftFile.success || !m_rightFile.success) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Sync internal state with latest editor content to prevent stale-data bugs
    m_leftFile.lines = m_leftPanel->editor()->toPlainText().split('\n');
    m_rightFile.lines = m_rightPanel->editor()->toPlainText().split('\n');

    m_diffResult = DiffEngine::diff(m_leftFile.lines, m_rightFile.lines);
    DiffHighlighter::apply(m_leftPanel->editor(), m_rightPanel->editor(), m_diffResult);

    m_scrollSync->reset();
    m_scrollSync->setEnabled(true);
    m_acceptAllAction->setEnabled(!m_diffResult.isIdentical());

    QApplication::restoreOverrideCursor();
    showDiffSummary(m_diffResult);
}

void MainWindow::onToggleTheme()
{
    ThemeManager::instance()->toggleTheme();
}

void MainWindow::onThemeChanged()
{
    IconManager::instance()->clearCache();
    refreshIcons();
    m_actTheme->setText(ThemeManager::instance()->isDark() ? "Light Mode" : "Dark Mode");
    if (!m_diffResult.hunks.isEmpty()) {
        DiffHighlighter::apply(m_leftPanel->editor(), m_rightPanel->editor(), m_diffResult);
    }
}

void MainWindow::refreshIcons()
{
    auto* im = IconManager::instance();
    const QSize sz(16, 16);
    const bool dark = ThemeManager::instance()->isDark();
    m_actOpenLeft->setIcon(im->icon(Icons::FolderOpen, sz));
    m_actOpenRight->setIcon(im->icon(Icons::FolderOpen, sz));
    m_actCompare->setIcon(im->icon(Icons::Compare, sz));
    m_actTheme->setIcon(im->icon(dark ? Icons::Sun : Icons::Moon, sz));

    m_acceptAllAction->setIcon(im->icon(Icons::Merge, sz));
    m_undoAction->setIcon(im->icon(Icons::Undo, sz));
    m_saveRightAction->setIcon(im->icon(Icons::Save, sz));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    event->accept();
}

void MainWindow::onAcceptAll()
{
    if (m_diffResult.hunks.isEmpty()) return;
    if (QMessageBox::question(this, "Accept All", "Replace right file content with left side changes?",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    DiffHighlighter::clear(m_leftPanel->editor(), m_rightPanel->editor());
    MergeEngine::acceptAll(m_rightPanel->editor(), m_diffResult);
    refreshAfterMerge();
    QApplication::restoreOverrideCursor();
}

void MainWindow::refreshAfterMerge()
{
    if (!m_leftFile.success) return;

    DiffHighlighter::clear(m_leftPanel->editor(), m_rightPanel->editor());

    // Sync state immediately after merge so caches are current
    m_leftFile.lines = m_leftPanel->editor()->toPlainText().split('\n');
    const QStringList rightLines = m_rightPanel->editor()->toPlainText().split('\n');
    m_rightFile.lines = rightLines;

    m_diffResult = DiffEngine::diff(m_leftFile.lines, rightLines);
    DiffHighlighter::apply(m_leftPanel->editor(), m_rightPanel->editor(), m_diffResult);

    m_acceptAllAction->setEnabled(!m_diffResult.isIdentical());
    showDiffSummary(m_diffResult);
}

void MainWindow::onSaveRight()
{
    if (!m_rightFile.success || m_rightFile.filePath.isEmpty()) return;

    QFile file(m_rightFile.filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Save Error", file.errorString());
        return;
    }

    QTextStream out(&file);
    out << m_rightPanel->editor()->toPlainText();
    file.close();

    m_rightPanel->editor()->document()->setModified(false);
    m_statusLabel->setText(QString("Saved: %1").arg(m_rightFile.filePath));
}
