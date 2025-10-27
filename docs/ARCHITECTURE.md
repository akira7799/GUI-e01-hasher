# E01 Hash Verification Tool - Architecture

## Class Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                             │
│                    (Application Entry)                       │
└──────────────────────────┬──────────────────────────────────┘
                           │ creates
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                       MainWindow                             │
│                   (QMainWindow derived)                      │
│                                                              │
│  Responsibilities:                                           │
│  - UI orchestration and layout management                   │
│  - State machine (READY→FILE_LOADED→VERIFYING→COMPLETE)    │
│  - Signal/slot connections between components               │
│  - Error dialog coordination                                │
│                                                              │
│  Key Slots:                                                  │
│  - onFileSelected(QString path)                             │
│  - onStartVerification()                                     │
│  - onCancelVerification()                                    │
│  - onVerificationComplete()                                  │
│  - onError(QString message)                                  │
└───┬────────────┬────────────┬────────────┬──────────────────┘
    │            │            │            │
    │ contains   │ contains   │ contains   │ contains
    ▼            ▼            ▼            ▼
┌─────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐
│DropZone │  │Metadata  │  │Progress  │  │Results   │
│Widget   │  │Display   │  │Widget    │  │Display   │
│         │  │Widget    │  │          │  │Widget    │
└─────────┘  └──────────┘  └──────────┘  └──────────┘
    │
    │ signals fileSelected
    ▼
┌─────────────────────────────────────────────────────────────┐
│                       EWFHandler                             │
│                   (libewf C API wrapper)                     │
│                                                              │
│  Responsibilities:                                           │
│  - Open/close E01/Ex01/DD files using libewf                │
│  - Read file metadata (case #, evidence ID, hashes)         │
│  - Provide sequential data reading interface                │
│  - Detect missing segments                                   │
│  - Handle libewf errors                                      │
│                                                              │
│  Key Methods:                                                │
│  - bool open(QString path)                                   │
│  - QMap<QString,QString> getMetadata()                      │
│  - qint64 read(char* buffer, qint64 size)                   │
│  - qint64 getTotalSize()                                     │
│  - void close()                                              │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ used by
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                      HashEngine                              │
│                    (QThread derived)                         │
│                                                              │
│  Responsibilities:                                           │
│  - Background hash calculation thread                        │
│  - Read data from EWFHandler in 1MB chunks                  │
│  - Calculate MD5, SHA1, SHA256 using Windows CryptoAPI      │
│  - Emit progress updates (percentage, time remaining)        │
│  - Handle cancellation requests                              │
│  - Compare calculated vs expected hashes                     │
│                                                              │
│  Key Methods:                                                │
│  - void run() override                                       │
│  - void cancel()                                             │
│                                                              │
│  Key Signals:                                                │
│  - progressUpdate(int percentage, QString timeRemaining)     │
│  - hashCalculated(QString algorithm, QString hash)           │
│  - verificationComplete(QMap<QString,bool> results)         │
│  - error(QString message)                                    │
└─────────────────────────────────────────────────────────────┘
```

## Component Details

### MainWindow (QMainWindow)
**Purpose**: Central UI controller that manages application state and coordinates all widgets.

**State Machine**:
- `READY`: Waiting for file input
- `FILE_LOADED`: File loaded, metadata displayed, ready to verify
- `VERIFYING`: Hash calculation in progress
- `COMPLETE`: Verification finished, results displayed

**UI Layout**:
```
┌────────────────────────────────────────┐
│  E01 Hash Verification Tool    [_][□][X]│
├────────────────────────────────────────┤
│                                        │
│  ┌──────────────────────────────────┐ │
│  │       Drop Zone Widget           │ │
│  │  "Drag E01 file here" [Browse]   │ │
│  └──────────────────────────────────┘ │
│                                        │
│  ┌──────────────────────────────────┐ │
│  │    Metadata Display Widget       │ │
│  │  Case #: [        ]              │ │
│  │  Evidence: [      ]              │ │
│  │  ☑ MD5  ☑ SHA1  ☑ SHA256        │ │
│  │           [Start Verification]   │ │
│  └──────────────────────────────────┘ │
│                                        │
│  ┌──────────────────────────────────┐ │
│  │     Progress Widget (hidden)     │ │
│  │  [████████████░░░░] 65%          │ │
│  │  Time remaining: 2m 15s [Cancel] │ │
│  └──────────────────────────────────┘ │
│                                        │
│  ┌──────────────────────────────────┐ │
│  │   Results Display (hidden)       │ │
│  │  ✓ MD5: Verified                 │ │
│  │  ✓ SHA1: Verified                │ │
│  │  ✓ SHA256: Verified              │ │
│  │  Completed in 5m 23s             │ │
│  └──────────────────────────────────┘ │
│                                        │
└────────────────────────────────────────┘
```

### EWFHandler (libewf wrapper)
**Purpose**: Encapsulate libewf C API into a clean C++ interface.

**Key Features**:
- RAII pattern (Resource Acquisition Is Initialization)
- Automatic cleanup on destruction
- Qt-friendly API (QString instead of char*)
- Error handling with descriptive messages

### HashEngine (QThread)
**Purpose**: Background worker thread for hash calculation to keep UI responsive.

**Threading Model**:
- Runs in separate QThread
- Communicates via Qt signals (thread-safe)
- Can be interrupted via QThread::requestInterruption()
- Uses Windows CryptoAPI for hash calculations

**Hash Calculation Flow**:
1. Initialize CryptoAPI contexts (MD5, SHA1, SHA256)
2. Read data in 1MB chunks from EWFHandler
3. Update hash contexts for each chunk
4. Calculate progress and emit signals
5. Finalize hashes and convert to hex strings
6. Compare with expected hashes from metadata
7. Emit results

### Custom Widgets

#### DropZone (QFrame)
- Drag-and-drop event handling
- File filter validation
- "Browse" button with QFileDialog
- Visual feedback on hover

#### MetadataDisplay (QGroupBox)
- QFormLayout for key-value pairs
- QCheckBox for hash algorithm selection
- "Start Verification" button

#### ProgressWidget (QWidget)
- QProgressBar
- QLabel for time remaining
- "Cancel" button

#### ResultsDisplay (QWidget)
- Icons (✓ or ✗) with QLabel
- Expected vs calculated hash display
- Total time elapsed

## Error Handling

All errors are handled through:
1. ErrorDialog class (QMessageBox-based)
2. MainWindow::onError() slot
3. State machine returns to READY state on error

## File Flow

```
User Action → DropZone → MainWindow::onFileSelected()
                             ↓
                        EWFHandler::open()
                             ↓
                        Display Metadata
                             ↓
                   User clicks "Start"
                             ↓
                    HashEngine::start()
                             ↓
              (Background thread running)
                             ↓
              Progress signals → ProgressWidget
                             ↓
              Complete signal → ResultsDisplay
```

## Dependencies

- **Qt Modules**: Core, GUI, Widgets
- **External Libraries**: libewf (static)
- **System APIs**: Windows CryptoAPI (advapi32.dll)

## Build Integration

All classes will be added to the Makefile incrementally as they are created.
