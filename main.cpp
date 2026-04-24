#include <QApplication>
#include <QIcon>
#include "ui/mainwindow.h"
#include "theme/thememanager.h"

int main(int argc, char *argv[])
{
    // Initialize the application
    QApplication app(argc, argv);
    app.setApplicationName("SmartDiff");
    app.setApplicationVersion("1.0.0");

    // Set the application icon
    app.setWindowIcon(QIcon(":/icons/app_icon.png"));

    // Apply the default application theme
    ThemeManager::instance()->applyTheme(ThemeManager::Theme::Dark);

    // Initialize and display the main window
    MainWindow window;
    window.show();

    // Start the main event loop
    return app.exec();
}
