/*
 * E01 Hash Verification Tool
 * MainWindow Implementation
 */

#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer>
#include <QHBoxLayout>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , dropZoneFrame(nullptr)
    , dropZoneLabel(nullptr)
    , browseButton(nullptr)
    , metadataGroup(nullptr)
    , metadataLabel(nullptr)
    , md5CheckBox(nullptr)
    , sha1CheckBox(nullptr)
    , sha256CheckBox(nullptr)
    , startButton(nullptr)
    , progressGroup(nullptr)
    , progressBar(nullptr)
    , progressLabel(nullptr)
    , cancelButton(nullptr)
    , resultsGroup(nullptr)
    , resultsLabel(nullptr)
    , ewfHandler(nullptr)
    , hashEngine(nullptr)
    , currentState(STATE_READY)
{
    // Initialize EWF handler
    ewfHandler = new EWFHandler();

    // Enable drag and drop
    setAcceptDrops(true);

    setupUI();
    createMenuBar();
    setState(STATE_READY);
}

MainWindow::~MainWindow()
{
    // Clean up hash engine if running
    if (hashEngine) {
        hashEngine->cancel();
        hashEngine->wait();
        delete hashEngine;
    }

    // Clean up EWF handler
    if (ewfHandler) {
        ewfHandler->close();
        delete ewfHandler;
    }
}

void MainWindow::setupUI()
{
    // Set window properties
    setWindowTitle("E01 Hash Verification Tool");
    setFixedSize(800, 600);

    // Create central widget and main layout
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // === Drop Zone Section ===
    dropZoneFrame = new QFrame(centralWidget);
    dropZoneFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    dropZoneFrame->setMinimumHeight(120);
    dropZoneFrame->setStyleSheet(
        "QFrame {"
        "  background-color: #f5f5f5;"
        "  border: 2px dashed #999;"
        "  border-radius: 8px;"
        "}"
    );

    QVBoxLayout *dropZoneLayout = new QVBoxLayout(dropZoneFrame);
    dropZoneLabel = new QLabel("Drag and drop an E01 file here", dropZoneFrame);
    dropZoneLabel->setAlignment(Qt::AlignCenter);
    dropZoneLabel->setStyleSheet("font-size: 14pt; color: #666;");

    browseButton = new QPushButton("Browse for File...", dropZoneFrame);
    browseButton->setMinimumHeight(35);
    browseButton->setMaximumWidth(200);
    connect(browseButton, &QPushButton::clicked, this, [this]() {
        QString filename = QFileDialog::getOpenFileName(
            this,
            "Select Forensic Image File",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
            "Forensic Images (*.E01 *.e01 *.Ex01 *.ex01 *.DD *.dd *.raw);;All Files (*.*)"
        );
        if (!filename.isEmpty()) {
            onFileSelected(filename);
        }
    });

    dropZoneLayout->addWidget(dropZoneLabel);
    dropZoneLayout->addWidget(browseButton, 0, Qt::AlignCenter);

    mainLayout->addWidget(dropZoneFrame);

    // === Metadata Display Section ===
    metadataGroup = new QGroupBox("File Metadata", centralWidget);
    metadataGroup->setMinimumHeight(200);
    metadataGroup->setVisible(false);

    QVBoxLayout *metadataLayout = new QVBoxLayout(metadataGroup);
    metadataLabel = new QLabel("No file loaded", metadataGroup);
    metadataLabel->setWordWrap(true);
    metadataLayout->addWidget(metadataLabel);

    // Hash algorithm checkboxes
    QLabel *hashSelectLabel = new QLabel("<b>Select hash algorithms to verify:</b>", metadataGroup);
    metadataLayout->addWidget(hashSelectLabel);

    QHBoxLayout *checkboxLayout = new QHBoxLayout();
    md5CheckBox = new QCheckBox("MD5", metadataGroup);
    sha1CheckBox = new QCheckBox("SHA1", metadataGroup);
    sha256CheckBox = new QCheckBox("SHA256", metadataGroup);

    md5CheckBox->setChecked(true);
    sha1CheckBox->setChecked(true);
    sha256CheckBox->setChecked(false);  // SHA256 off by default

    checkboxLayout->addWidget(md5CheckBox);
    checkboxLayout->addWidget(sha1CheckBox);
    checkboxLayout->addWidget(sha256CheckBox);
    checkboxLayout->addStretch();

    metadataLayout->addLayout(checkboxLayout);

    // Start button
    startButton = new QPushButton("Start Verification", metadataGroup);
    startButton->setMinimumHeight(35);
    startButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartVerification);

    metadataLayout->addWidget(startButton);

    mainLayout->addWidget(metadataGroup);

    // === Progress Section ===
    progressGroup = new QGroupBox("Verification Progress", centralWidget);
    progressGroup->setMinimumHeight(120);
    progressGroup->setVisible(false);

    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);

    progressBar = new QProgressBar(progressGroup);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressLayout->addWidget(progressBar);

    progressLabel = new QLabel("Starting verification...", progressGroup);
    progressLayout->addWidget(progressLabel);

    cancelButton = new QPushButton("Cancel", progressGroup);
    cancelButton->setMaximumWidth(100);
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelVerification);
    progressLayout->addWidget(cancelButton, 0, Qt::AlignRight);

    mainLayout->addWidget(progressGroup);

    // === Results Section ===
    resultsGroup = new QGroupBox("Verification Results", centralWidget);
    resultsGroup->setMinimumHeight(150);
    resultsGroup->setVisible(false);

    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroup);
    resultsLabel = new QLabel("No results yet", resultsGroup);
    resultsLabel->setWordWrap(true);
    resultsLayout->addWidget(resultsLabel);

    mainLayout->addWidget(resultsGroup);

    // Add stretch to push everything to the top
    mainLayout->addStretch();

    // Set central widget
    setCentralWidget(centralWidget);
}

void MainWindow::createMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File Menu
    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *openAction = new QAction("&Open File...", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, [this]() {
        browseButton->click();
    });
    fileMenu->addAction(openAction);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);

    // Help Menu
    QMenu *helpMenu = menuBar->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About E01 Hash Verification Tool",
            "<h3>E01 Hash Verification Tool</h3>"
            "<p>Version 1.0.0</p>"
            "<p>Forensic image hash verification utility</p>"
            "<p>Supports E01, Ex01, DD, and other libewf-compatible formats</p>"
            "<br>"
            "<p>Built with Qt " + QString(qVersion()) + "</p>"
            "<p>Using libewf " + QString(libewf_get_version()) + "</p>"
        );
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::setState(ApplicationState newState)
{
    currentState = newState;

    // Update UI based on state
    switch (currentState) {
        case STATE_READY:
            dropZoneFrame->setVisible(true);
            metadataGroup->setVisible(false);
            progressGroup->setVisible(false);
            resultsGroup->setVisible(false);
            break;

        case STATE_FILE_LOADED:
            dropZoneFrame->setVisible(false);
            metadataGroup->setVisible(true);
            progressGroup->setVisible(false);
            resultsGroup->setVisible(false);
            startButton->setEnabled(true);
            break;

        case STATE_VERIFYING:
            dropZoneFrame->setVisible(false);
            metadataGroup->setVisible(true);
            progressGroup->setVisible(true);
            resultsGroup->setVisible(false);
            startButton->setEnabled(false);
            break;

        case STATE_COMPLETE:
            dropZoneFrame->setVisible(false);
            metadataGroup->setVisible(true);
            progressGroup->setVisible(false);
            resultsGroup->setVisible(true);
            startButton->setEnabled(true);
            startButton->setText("Verify Another File");
            break;
    }
}

// ===== Slot Implementations =====

void MainWindow::onFileSelected(const QString &path)
{
    currentFilePath = path;

    // Close any previously opened file
    if (ewfHandler->isOpen()) {
        ewfHandler->close();
    }

    // Open the file
    if (!ewfHandler->open(path)) {
        onError("Failed to open file:\n" + ewfHandler->getLastError());
        return;
    }

    // Get metadata
    QMap<QString, QString> metadata = ewfHandler->getMetadata();

    // Build metadata display
    QString metadataText = "<b>File:</b> " + path + "<br><br>";

    QString caseNum = metadata.value("case_number");
    QString evidenceNum = metadata.value("evidence_number");
    QString description = metadata.value("description");
    QString examiner = metadata.value("examiner_name");

    if (!caseNum.isEmpty()) {
        metadataText += "<b>Case Number:</b> " + caseNum + "<br>";
    }
    if (!evidenceNum.isEmpty()) {
        metadataText += "<b>Evidence Number:</b> " + evidenceNum + "<br>";
    }
    if (!description.isEmpty()) {
        metadataText += "<b>Description:</b> " + description + "<br>";
    }
    if (!examiner.isEmpty()) {
        metadataText += "<b>Examiner:</b> " + examiner + "<br>";
    }

    metadataText += "<br><b>Media Size:</b> " + QString::number(ewfHandler->getMediaSize() / (1024*1024)) + " MB<br>";

    // Check for stored hashes
    expectedMD5 = metadata.value("stored_md5");
    expectedSHA1 = metadata.value("stored_sha1");

    if (!expectedMD5.isEmpty()) {
        metadataText += "<br><b>Stored MD5:</b> " + expectedMD5 + "<br>";
        md5CheckBox->setChecked(true);
    }
    if (!expectedSHA1.isEmpty()) {
        metadataText += "<b>Stored SHA1:</b> " + expectedSHA1 + "<br>";
        sha1CheckBox->setChecked(true);
    }

    metadataLabel->setText(metadataText);

    setState(STATE_FILE_LOADED);
}

void MainWindow::onStartVerification()
{
    // If we're in STATE_COMPLETE, reset to load another file
    if (currentState == STATE_COMPLETE) {
        ewfHandler->close();
        currentFilePath.clear();
        calculatedMD5.clear();
        calculatedSHA1.clear();
        calculatedSHA256.clear();
        expectedMD5.clear();
        expectedSHA1.clear();
        expectedSHA256.clear();
        startButton->setText("Start Verification");
        setState(STATE_READY);
        return;
    }

    // Clean up old hash engine if exists
    if (hashEngine) {
        hashEngine->wait();
        delete hashEngine;
    }

    // Create new hash engine
    hashEngine = new HashEngine(ewfHandler, this);

    // Connect signals
    connect(hashEngine, &HashEngine::progressUpdate, this, &MainWindow::onProgressUpdate);
    connect(hashEngine, &HashEngine::timeEstimate, this, &MainWindow::onTimeEstimate);
    connect(hashEngine, &HashEngine::md5Calculated, this, &MainWindow::onMD5Calculated);
    connect(hashEngine, &HashEngine::sha1Calculated, this, &MainWindow::onSHA1Calculated);
    connect(hashEngine, &HashEngine::sha256Calculated, this, &MainWindow::onSHA256Calculated);
    connect(hashEngine, &HashEngine::verificationComplete, this, &MainWindow::onVerificationComplete);
    connect(hashEngine, &HashEngine::error, this, &MainWindow::onHashError);

    // Configure hash algorithms
    hashEngine->enableMD5(md5CheckBox->isChecked());
    hashEngine->enableSHA1(sha1CheckBox->isChecked());
    hashEngine->enableSHA256(sha256CheckBox->isChecked());

    // Set expected hashes
    if (!expectedMD5.isEmpty()) {
        hashEngine->setExpectedMD5(expectedMD5);
    }
    if (!expectedSHA1.isEmpty()) {
        hashEngine->setExpectedSHA1(expectedSHA1);
    }

    // Clear previous results
    calculatedMD5.clear();
    calculatedSHA1.clear();
    calculatedSHA256.clear();

    // Reset progress
    progressBar->setValue(0);
    progressLabel->setText("Starting verification...");

    // Update state
    setState(STATE_VERIFYING);

    // Start hash calculation
    hashEngine->start();
}

void MainWindow::onCancelVerification()
{
    int result = QMessageBox::question(this, "Cancel Verification",
        "Are you sure you want to cancel the verification?",
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes && hashEngine) {
        hashEngine->cancel();
        hashEngine->wait();

        setState(STATE_FILE_LOADED);
        QMessageBox::information(this, "Cancelled", "Verification cancelled by user.");
    }
}

void MainWindow::onProgressUpdate(int percentage, qint64 bytesProcessed, qint64 totalBytes)
{
    progressBar->setValue(percentage);

    QString progressText = QString("Processing: %1 / %2 MB (%3%)")
        .arg(bytesProcessed / (1024*1024))
        .arg(totalBytes / (1024*1024))
        .arg(percentage);

    progressLabel->setText(progressText);
}

void MainWindow::onTimeEstimate(const QString &remaining)
{
    QString currentText = progressLabel->text();
    progressLabel->setText(currentText + " - Time remaining: " + remaining);
}

void MainWindow::onMD5Calculated(const QString &hash)
{
    calculatedMD5 = hash;
}

void MainWindow::onSHA1Calculated(const QString &hash)
{
    calculatedSHA1 = hash;
}

void MainWindow::onSHA256Calculated(const QString &hash)
{
    calculatedSHA256 = hash;
}

void MainWindow::onVerificationComplete(const QMap<QString, bool> &results)
{
    setState(STATE_COMPLETE);

    QString resultsText = "<h3>Verification Complete</h3><br>";

    bool allPassed = true;

    // Display MD5 result
    if (md5CheckBox->isChecked()) {
        bool verified = results.value("MD5", false);
        if (!expectedMD5.isEmpty()) {
            if (verified) {
                resultsText += "<span style='color: green;'><b>✓ MD5: Verified</b></span><br>";
                resultsText += "  Expected:   " + expectedMD5 + "<br>";
                resultsText += "  Calculated: " + calculatedMD5 + "<br><br>";
            } else {
                resultsText += "<span style='color: red;'><b>✗ MD5: NOT VERIFIED</b></span><br>";
                resultsText += "  Expected:   " + expectedMD5 + "<br>";
                resultsText += "  Calculated: " + calculatedMD5 + "<br><br>";
                allPassed = false;
            }
        } else {
            resultsText += "<b>MD5:</b> " + calculatedMD5 + "<br>";
            resultsText += "  (No stored hash to compare)<br><br>";
        }
    }

    // Display SHA1 result
    if (sha1CheckBox->isChecked()) {
        bool verified = results.value("SHA1", false);
        if (!expectedSHA1.isEmpty()) {
            if (verified) {
                resultsText += "<span style='color: green;'><b>✓ SHA1: Verified</b></span><br>";
                resultsText += "  Expected:   " + expectedSHA1 + "<br>";
                resultsText += "  Calculated: " + calculatedSHA1 + "<br><br>";
            } else {
                resultsText += "<span style='color: red;'><b>✗ SHA1: NOT VERIFIED</b></span><br>";
                resultsText += "  Expected:   " + expectedSHA1 + "<br>";
                resultsText += "  Calculated: " + calculatedSHA1 + "<br><br>";
                allPassed = false;
            }
        } else {
            resultsText += "<b>SHA1:</b> " + calculatedSHA1 + "<br>";
            resultsText += "  (No stored hash to compare)<br><br>";
        }
    }

    // Display SHA256 result
    if (sha256CheckBox->isChecked()) {
        resultsText += "<b>SHA256:</b> " + calculatedSHA256 + "<br>";
        resultsText += "  (No stored hash to compare)<br><br>";
    }

    resultsLabel->setText(resultsText);

    // Show completion message
    if (allPassed) {
        QMessageBox::information(this, "Verification Complete",
            "Hash verification completed successfully!\n\nAll hashes match the stored values.");
    } else {
        QMessageBox::warning(this, "Verification Failed",
            "Hash verification completed with mismatches!\n\nPlease check the results.");
    }
}

void MainWindow::onHashError(const QString &message)
{
    onError("Hash calculation error: " + message);
}

void MainWindow::onError(const QString &message)
{
    QMessageBox::critical(this, "Error", message);
    setState(STATE_READY);
}

// ===== Drag and Drop Event Handlers =====

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Only accept if drag contains URLs (files)
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();

        // Accept only if exactly one file is being dragged
        if (urls.size() == 1) {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);

            // Reject directories, only accept valid forensic files
            if (fileInfo.isFile() && isValidForensicFile(filePath)) {
                event->acceptProposedAction();

                // Update drop zone visual feedback
                if (dropZoneFrame && currentState == STATE_READY) {
                    dropZoneFrame->setStyleSheet(
                        "QFrame {"
                        "  background-color: #e3f2fd;"
                        "  border: 2px dashed #2196F3;"
                        "  border-radius: 8px;"
                        "}"
                    );
                }
                return;
            }
        }
    }

    event->ignore();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // Same logic as dragEnterEvent
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();

        if (urls.size() == 1) {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);

            if (fileInfo.isFile() && isValidForensicFile(filePath)) {
                event->acceptProposedAction();
                return;
            }
        }
    }

    event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    // Reset drop zone styling
    if (dropZoneFrame) {
        dropZoneFrame->setStyleSheet(
            "QFrame {"
            "  background-color: #f5f5f5;"
            "  border: 2px dashed #999;"
            "  border-radius: 8px;"
            "}"
        );
    }

    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();

        if (urls.size() == 1) {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);

            if (fileInfo.isFile() && isValidForensicFile(filePath)) {
                event->acceptProposedAction();
                onFileSelected(filePath);
                return;
            }
        }
    }

    event->ignore();
}

bool MainWindow::isValidForensicFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    // Accepted forensic image extensions
    QStringList validExtensions = {
        "e01", "e02", "e03", "e04", "e05", "e06", "e07", "e08", "e09",
        "ex01", "ex02", "ex03", "ex04", "ex05",
        "dd", "raw", "img", "bin"
    };

    return validExtensions.contains(extension);
}
