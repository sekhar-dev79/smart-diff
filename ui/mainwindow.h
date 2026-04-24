#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QLabel>
#include <QAction>

#include "filepanel.h"
#include "fileloader.h"
#include "diffmodel.h"
#include "scrollsynchronizer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onOpenLeftFile();
    void onOpenRightFile();
    void onCompare();
    void onToggleTheme();
    void onThemeChanged();
    void onAcceptAll();
    void onSaveRight();

private:
    void buildToolbar();
    void buildCentralWidget();
    void buildStatusBar();
    void refreshIcons();
    void applyFileResult(FilePanel* panel, const FileResult& result, bool isLeft);
    void updateCompareButton();
    void showDiffSummary(const DiffResult& result);
    void refreshAfterMerge();

    // Toolbar actions
    QAction* m_actOpenLeft     { nullptr };
    QAction* m_actOpenRight    { nullptr };
    QAction* m_actCompare      { nullptr };
    QAction* m_actTheme        { nullptr };
    QAction* m_acceptAllAction { nullptr };
    QAction* m_saveRightAction { nullptr };
    QAction* m_undoAction      { nullptr };

    // Layout components
    QSplitter* m_splitter      { nullptr };
    FilePanel* m_leftPanel     { nullptr };
    FilePanel* m_rightPanel    { nullptr };
    QLabel* m_statusLabel   { nullptr };

    // State management
    FileResult m_leftFile;
    FileResult m_rightFile;
    DiffResult m_diffResult;

    ScrollSynchronizer* m_scrollSync { nullptr };
    QString m_lastDirectory;
};
