/*
 * E01 Hash Verification Tool
 * Main application entry point
 */

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("E01 Hash Verification Tool");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Forensic Tools");

    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
