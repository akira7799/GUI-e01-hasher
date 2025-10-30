/*
 * E01 Hash Verification Tool
 * Main application entry point
 */

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QMap>
#include <QString>
#include <QMetaType>

int main(int argc, char *argv[])
{
    // Register custom types for cross-thread signal/slot communication
    qRegisterMetaType<QMap<QString,bool>>("QMap<QString,bool>");

    QApplication app(argc, argv);

    app.setApplicationName("E01 Hash Verification Tool");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Forensic Tools");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
