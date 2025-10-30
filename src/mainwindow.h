/*
 * E01 Hash Verification Tool
 * MainWindow - Main application window and UI controller
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QCheckBox>
#include <QProgressBar>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include "ewfhandler.h"
#include "hashengine.h"

// Forward declarations for future widgets
// class DropZone;
// class MetadataDisplay;
// class ProgressWidget;
// class ResultsDisplay;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Application states
    enum ApplicationState {
        STATE_READY,           // Waiting for file input
        STATE_FILE_LOADED,     // File loaded, metadata displayed
        STATE_VERIFYING,       // Hash calculation in progress
        STATE_COMPLETE         // Verification finished
    };

private slots:
    // File handling
    void onFileSelected(const QString &path);
    void onStartVerification();
    void onCancelVerification();

    // Hash engine signals
    void onProgressUpdate(int percentage, qint64 bytesProcessed, qint64 totalBytes);
    void onTimeEstimate(const QString &remaining);
    void onMD5Calculated(const QString &hash);
    void onSHA1Calculated(const QString &hash);
    void onSHA256Calculated(const QString &hash);
    void onVerificationComplete(const QMap<QString, bool> &results);
    void onHashError(const QString &message);

    // General error handling
    void onError(const QString &message);

protected:
    // Drag and drop events
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setupUI();
    void createMenuBar();
    void setState(ApplicationState newState);
    bool isValidForensicFile(const QString &filePath);

    // UI Components (placeholders for now)
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;

    // Placeholder widgets (will be replaced with custom widgets later)
    QFrame *dropZoneFrame;
    QLabel *dropZoneLabel;
    QPushButton *browseButton;

    QGroupBox *metadataGroup;
    QLabel *metadataLabel;
    QCheckBox *md5CheckBox;
    QCheckBox *sha1CheckBox;
    QCheckBox *sha256CheckBox;
    QPushButton *startButton;

    QGroupBox *progressGroup;
    QProgressBar *progressBar;
    QLabel *progressLabel;
    QPushButton *cancelButton;

    QGroupBox *resultsGroup;
    QLabel *resultsLabel;

    // Core components
    EWFHandler *ewfHandler;
    HashEngine *hashEngine;

    // State management
    ApplicationState currentState;
    QString currentFilePath;

    // Calculated and expected hashes
    QString calculatedMD5;
    QString calculatedSHA1;
    QString calculatedSHA256;
    QString expectedMD5;
    QString expectedSHA1;
    QString expectedSHA256;
};

#endif // MAINWINDOW_H
