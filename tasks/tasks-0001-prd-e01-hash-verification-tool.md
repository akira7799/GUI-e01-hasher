# Task List: E01 Hash Verification Tool

Generated from: `0001-prd-e01-hash-verification-tool.md`

## Relevant Files

### Core Application Files
- `src/main.cpp` - Application entry point, initializes Qt application and MainWindow
- `src/mainwindow.h` - MainWindow class header with UI component declarations
- `src/mainwindow.cpp` - MainWindow implementation with GUI logic and event handling
- `src/mainwindow.ui` - Qt Designer UI file for MainWindow layout

### Hash Engine and LibEWF Integration
- `src/hashengine.h` - HashEngine class header for hash calculation logic
- `src/hashengine.cpp` - Implementation of hash calculation with threading support
- `src/ewfhandler.h` - Wrapper class header for libewf library integration
- `src/ewfhandler.cpp` - Implementation of E01/Ex01 file handling and metadata extraction

### GUI Components
- `src/widgets/dropzone.h` - Custom widget for drag-and-drop file input
- `src/widgets/dropzone.cpp` - Implementation of drag-and-drop functionality
- `src/widgets/metadatadisplay.h` - Widget for displaying E01 metadata
- `src/widgets/metadatadisplay.cpp` - Implementation of metadata display panel
- `src/widgets/resultsdisplay.h` - Widget for displaying verification results with visual indicators
- `src/widgets/resultsdisplay.cpp` - Implementation of results panel with green/red indicators
- `src/widgets/progresswidget.h` - Widget for progress bar and time estimation
- `src/widgets/progresswidget.cpp` - Implementation of progress tracking UI

### Utility and Support Files
- `src/utils/fileutils.h` - Utility functions for file validation and path handling
- `src/utils/fileutils.cpp` - Implementation of file utility functions
- `src/utils/hashutils.h` - Hash algorithm utilities and string formatting
- `src/utils/hashutils.cpp` - Implementation of hash utilities
- `src/utils/logger.h` - Optional logging functionality (if implemented)
- `src/utils/logger.cpp` - Optional logger implementation

### Dialogs
- `src/dialogs/errordialog.h` - Custom error dialog class
- `src/dialogs/errordialog.cpp` - Implementation of error message dialogs
- `src/dialogs/canceldialog.h` - Confirmation dialog for cancellation
- `src/dialogs/canceldialog.cpp` - Implementation of cancel confirmation

### Resources
- `resources/application.qrc` - Qt resource file containing icons and images
- `resources/icons/app_icon.ico` - Application icon for Windows executable
- `resources/icons/verified.png` - Green checkmark icon for verified hashes
- `resources/icons/not_verified.png` - Red X icon for failed verification
- `resources/icons/warning.png` - Warning icon for error dialogs

### Build System
- `e01hasher.pro` - Qt project file (qmake) with static linking configuration
- `CMakeLists.txt` - Alternative CMake build configuration (if using CMake)
- `build_static_qt.sh` - Script for building static Qt on Windows/WSL
- `build_libewf.sh` - Script for building static libewf library

### Installer
- `installer/e01hasher.wxs` - WiX installer source file for MSI creation
- `installer/banner.bmp` - Installer banner image
- `installer/dialog.bmp` - Installer dialog background image
- `installer/license.rtf` - License agreement for installer (MIT or Apache 2.0)

### Documentation
- `README.md` - Project overview, build instructions, and usage guide
- `LICENSE` - MIT or Apache 2.0 license file
- `BUILDING.md` - Detailed instructions for building static dependencies
- `CHANGELOG.md` - Version history and release notes

### Testing Resources
- `tests/test_cases.md` - Documentation of test scenarios and expected results
- `tests/sample_files/README.md` - Instructions for obtaining test E01/Ex01 files

### Notes
- This is a greenfield C++ Qt project; all files will be newly created
- Use Qt Creator or Visual Studio with Qt extension for development
- Static linking requires custom-built Qt, libewf, and OpenSSL libraries
- No unit testing framework specified; manual testing with real forensic images will be primary QA method

## Tasks

- [x] 1.0 Development Environment Setup and Dependency Building
  - [x] 1.1 Install Visual Studio 2019/2022 with C++ desktop development workload (includes MSVC compiler and Windows SDK) - ADAPTED: MinGW-w64 GCC 13 cross-compiler installed
  - [x] 1.2 Install Qt Creator IDE (latest version) for Qt development on Windows - ADAPTED: Command-line build using qmake, Qt Creator not required
  - [x] 1.3 Install CMake (3.20+) and add to system PATH for build configuration - CMake 3.28.3 installed
  - [x] 1.4 Install Git for version control and download MinGW-w64 toolchain if needed - Git 2.43.0 and MinGW-w64 installed
  - [x] 1.5 Download Qt 5.15 or Qt 6.x source code from official Qt archives - Qt 5.15.2 downloaded and extracted to /tmp/qt-src/
  - [x] 1.6 Build static Qt libraries for Windows using configure script with `-static -release -prefix` options (see PRD Technical Considerations for compile flags) - ADAPTED: Used MSYS2 pre-built Qt 5.15.17 static libraries installed to /tmp/qt5-static-install/
  - [x] 1.7 Download and build static libewf library from source (GitHub: libyal/libewf) with Visual Studio or MinGW - Built with MinGW-w64 cross-compiler and installed to /tmp/libewf-install/
  - [x] 1.8 Build static OpenSSL library (1.1.1 or 3.x) for Windows OR verify Windows CryptoAPI availability as alternative - CHOSEN: Windows CryptoAPI verified with MinGW-w64 (MD5, SHA1, SHA256 supported, link with -ladvapi32)
  - [x] 1.9 Document build paths and create environment variables (QT_STATIC_DIR, LIBEWF_DIR, OPENSSL_DIR) for project configuration - Created /tmp/e01-hasher-build-env.sh and /home/dave/e01-hasher/BUILDING.md
  - [x] 1.10 Verify all static libraries are built successfully by checking for .lib/.a files and running test compile - Verified Qt (Core/Gui/Widgets), libewf, qmake, and MinGW-w64 toolchain

- [ ] 2.0 Project Initialization and Build System Configuration
  - [ ] 2.1 Create project root directory structure: `src/`, `include/`, `resources/`, `installer/`, `tests/`, `docs/`
  - [ ] 2.2 Initialize Git repository with .gitignore for Qt/C++ projects (exclude build directories, .user files, .o/.obj files)
  - [ ] 2.3 Create `e01hasher.pro` (qmake) with static linking configuration: `CONFIG += static`, library paths for libewf/OpenSSL
  - [ ] 2.4 Configure project for Windows-only deployment: `win32:` scope, set RC_ICONS for Windows executable icon
  - [ ] 2.5 Add Qt modules to .pro file: `QT += core gui widgets` (minimum required modules)
  - [ ] 2.6 Link static libraries in .pro file: `LIBS += -L$$LIBEWF_DIR/lib -lewf -L$$OPENSSL_DIR/lib -lcrypto`
  - [ ] 2.7 Create `src/main.cpp` with minimal Qt application boilerplate (QApplication, QMainWindow, exec())
  - [ ] 2.8 Build and verify minimal "Hello World" Qt application compiles and runs as standalone executable (no DLL dependencies)
  - [ ] 2.9 Use Dependency Walker or `dumpbin /dependents` to verify executable has no external DLL dependencies except system DLLs
  - [ ] 2.10 Create README.md with project overview and BUILDING.md with dependency build instructions

- [ ] 3.0 Core Application Architecture and Main Window
  - [ ] 3.1 Design class architecture diagram: MainWindow (UI controller), HashEngine (worker), EWFHandler (libewf wrapper), supporting widgets
  - [ ] 3.2 Create `src/mainwindow.h` and `src/mainwindow.cpp` classes inheriting from QMainWindow
  - [ ] 3.3 Create `src/mainwindow.ui` in Qt Designer with vertical layout: top metadata section, middle drop zone, bottom results section
  - [ ] 3.4 Set MainWindow properties: fixed size window (800x600px recommended), application title "E01 Hash Verification Tool"
  - [ ] 3.5 Create `resources/application.qrc` resource file and add to .pro file with `RESOURCES += resources/application.qrc`
  - [ ] 3.6 Add application icon (`app_icon.ico`) to resources and set as window icon in MainWindow constructor
  - [ ] 3.7 Implement MainWindow constructor to initialize UI components and connect signals/slots for future event handling
  - [ ] 3.8 Create placeholder slots for future implementation: `onFileSelected()`, `onStartVerification()`, `onCancelVerification()`
  - [ ] 3.9 Build and test that MainWindow displays correctly with placeholder UI elements
  - [ ] 3.10 Verify application icon appears in window title bar and taskbar

- [ ] 4.0 LibEWF Integration and Hash Calculation Engine
  - [ ] 4.1 Create `src/ewfhandler.h` with class declaration: methods for `openFile()`, `getMetadata()`, `readData()`, `close()`
  - [ ] 4.2 Implement `src/ewfhandler.cpp` with libewf API calls: `libewf_handle_open()`, `libewf_handle_get_utf8_header_value()` for metadata
  - [ ] 4.3 Add automatic segment detection: parse E01 filename, check for E02/E03 existence, pass segment list to libewf
  - [ ] 4.4 Implement metadata extraction: case number, evidence ID, description, examiner name, encoded MD5/SHA1/SHA256 hashes
  - [ ] 4.5 Add error handling in EWFHandler: return error codes for missing files, corrupted headers, unsupported formats
  - [ ] 4.6 Create `src/hashengine.h` with class declaration inheriting from QThread for background processing
  - [ ] 4.7 Implement `src/hashengine.cpp` with hash calculation logic: create OpenSSL contexts (MD5_CTX, SHA_CTX, SHA256_CTX) or Windows CryptoAPI handles
  - [ ] 4.8 Implement `HashEngine::run()` method: read data from EWFHandler in chunks (e.g., 1MB), update hash contexts, emit progress signals
  - [ ] 4.9 Add progress calculation: track bytes processed vs total file size, calculate percentage, estimate time remaining based on throughput
  - [ ] 4.10 Implement hash finalization: convert binary hashes to lowercase hexadecimal strings, emit results via signals
  - [ ] 4.11 Add cancellation support: check for QThread interruption flag periodically, clean up resources on cancel
  - [ ] 4.12 Test hash calculation against known E01 files: verify MD5/SHA1/SHA256 match libewf CLI tool output

- [ ] 5.0 GUI Implementation and User Interaction
  - [ ] 5.1 Create `src/widgets/dropzone.h` and `.cpp` custom widget inheriting from QFrame with drag-and-drop support
  - [ ] 5.2 Implement drag-and-drop event handlers: `dragEnterEvent()`, `dragMoveEvent()`, `dropEvent()` accepting single files only (reject folders)
  - [ ] 5.3 Add file filter validation in drop zone: accept .E01, .Ex01, .DD, .ZIP extensions (case-insensitive)
  - [ ] 5.4 Create "Browse" QPushButton in drop zone that opens QFileDialog with forensic image filters
  - [ ] 5.5 Emit `fileSelected(QString path)` signal when file is dropped or browsed, connect to MainWindow slot
  - [ ] 5.6 Create `src/widgets/metadatadisplay.h` and `.cpp` widget using QGroupBox with QFormLayout for key-value pairs
  - [ ] 5.7 Implement `MetadataDisplay::setMetadata(QMap<QString, QString>)` to populate labels: Case Number, Evidence ID, Description, Examiner, Encoded Hashes
  - [ ] 5.8 Add three QCheckBox widgets for hash algorithm selection: "MD5", "SHA1", "SHA256" with auto-selection based on encoded hashes
  - [ ] 5.9 Create "Start Verification" QPushButton (enabled only when file is loaded and at least one hash is selected)
  - [ ] 5.10 Create `src/widgets/progresswidget.h` and `.cpp` with QProgressBar and QLabel for percentage and time remaining
  - [ ] 5.11 Implement `ProgressWidget::updateProgress(int percentage, QString timeRemaining)` slot connected to HashEngine signals
  - [ ] 5.12 Add "Cancel" QPushButton in progress widget (visible only during verification) that shows confirmation dialog
  - [ ] 5.13 Create `src/dialogs/canceldialog.h` and `.cpp` with QMessageBox confirmation: "Are you sure you want to cancel verification?"
  - [ ] 5.14 Create `src/widgets/resultsdisplay.h` and `.cpp` widget to show verification results with visual indicators
  - [ ] 5.15 Implement results display with QLabel and QIcon for each hash: green checkmark (verified), red X (not verified), or calculated hash only
  - [ ] 5.16 Add "Expected" vs "Calculated" hash display for mismatches, show warning message for failures
  - [ ] 5.17 Display total verification time ("Verification completed in X minutes Y seconds") in results widget
  - [ ] 5.18 Connect all widgets in MainWindow layout: drop zone at top, metadata display below, progress widget (hidden initially), results display (hidden initially)
  - [ ] 5.19 Implement state machine in MainWindow: READY → FILE_LOADED → VERIFYING → COMPLETE → READY (on new file load)
  - [ ] 5.20 Test complete UI workflow: drag file, see metadata, select hashes, start verification, view progress, see results

- [ ] 6.0 Error Handling and Edge Case Management
  - [ ] 6.1 Create `src/dialogs/errordialog.h` and `.cpp` custom error dialog with QMessageBox styling and warning icon
  - [ ] 6.2 Implement missing segment detection in EWFHandler: check for sequential E01, E02, E03... files, identify first missing segment number
  - [ ] 6.3 Display error dialog on missing segment: "Missing segment. File e## is missing" (replace ## with actual segment number)
  - [ ] 6.4 Implement corrupted file detection: catch libewf errors during file open, header parsing, or data reading
  - [ ] 6.5 Display error dialog on corruption: "Corrupted File Encountered. Cannot perform hash verification."
  - [ ] 6.6 Implement file permission checking: attempt to open file with read access, catch permission denied errors
  - [ ] 6.7 Display error dialog on permission failure: "You have insufficient permission to hash this file. Try running the program as Administrator."
  - [ ] 6.8 Add error state handling: discard partial results on error, return UI to READY state for new file input
  - [ ] 6.9 Implement optional logging (FR-8): create `src/utils/logger.h` and `.cpp` with file-based logging to `%APPDATA%\E01HashTool\logs\`
  - [ ] 6.10 Add logging for key events: file loaded, verification started, errors encountered, results (if logging enabled in settings)
  - [ ] 6.11 Test all error scenarios: remove segment file, provide corrupted E01, restrict file permissions, verify correct error messages
  - [ ] 6.12 Test cancellation workflow: cancel mid-verification, verify confirmation dialog, check that partial results are discarded

- [ ] 7.0 Testing and Quality Assurance
  - [ ] 7.1 Obtain or create test E01/Ex01 files: small test image (< 100MB), medium image (1-5GB), large image (10GB+)
  - [ ] 7.2 Create test segmented E01 file set (E01, E02, E03) with known hash values for validation
  - [ ] 7.3 Create or obtain raw DD image file for testing non-E01 format support
  - [ ] 7.4 Test monolithic E01 file verification: verify all three hashes (MD5, SHA1, SHA256) match expected values
  - [ ] 7.5 Test segmented E01 file verification: verify tool correctly loads all segments and calculates correct hashes
  - [ ] 7.6 Test DD image verification: verify tool handles format without encoded metadata, only calculates selected hashes
  - [ ] 7.7 Test hash mismatch scenario: modify test E01 metadata to have incorrect hash, verify red X appears for mismatch
  - [ ] 7.8 Test missing segment error: remove E02 file from segmented set, verify correct error message with segment number
  - [ ] 7.9 Test corrupted file error: create corrupted E01 file (truncated or binary garbage), verify error handling
  - [ ] 7.10 Test permission error: set test file to read-restricted, run tool as non-admin, verify permission error message
  - [ ] 7.11 Test cancellation: start verification of large file, cancel mid-process, verify UI returns to ready state
  - [ ] 7.12 Test sequential verification workflow: verify file A, immediately drag file B, verify file B replaces file A session
  - [ ] 7.13 Performance benchmark: compare hash calculation speed against libewf CLI tool (`ewfverify`) on same hardware
  - [ ] 7.14 Verify performance meets success criteria: tool matches or exceeds CLI tool speed (within 5% acceptable)
  - [ ] 7.15 Memory leak testing: run multiple verification cycles on large files, monitor memory usage with Task Manager or Process Explorer
  - [ ] 7.16 UI responsiveness testing: verify progress bar updates smoothly (1+ update/sec), UI doesn't freeze during hash calculation
  - [ ] 7.17 Test automatic hash selection: load E01 with only MD5 encoded, verify only MD5 checkbox is auto-selected
  - [ ] 7.18 Test manual hash selection: auto-select MD5, manually add SHA256, verify both are calculated
  - [ ] 7.19 Test edge case: extremely large file (1TB+ if available), verify time estimation is reasonable, no overflow errors
  - [ ] 7.20 Document all test results in `tests/test_cases.md` with pass/fail status and notes

- [ ] 8.0 MSI Installer Creation and Packaging
  - [ ] 8.1 Choose installer technology: WiX Toolset v3/v4 (recommended for MSI), or Qt Installer Framework as alternative
  - [ ] 8.2 Install WiX Toolset and add to system PATH for command-line access to candle.exe and light.exe
  - [ ] 8.3 Create `installer/e01hasher.wxs` WiX source file with Product, Package, and Directory elements
  - [ ] 8.4 Define installation directory: `ProgramFilesFolder\E01 Hash Verification Tool\` as default install location
  - [ ] 8.5 Add File component in WiX: include statically-linked executable `e01hasher.exe` from build output
  - [ ] 8.6 Set application metadata in WiX: Product Name, Manufacturer, Version (1.0.0), Description, Icon
  - [ ] 8.7 Create Start Menu shortcut: add Shortcut element under ProgramMenuFolder with icon reference
  - [ ] 8.8 Create Desktop shortcut (optional): add Shortcut element under DesktopFolder if user selects option
  - [ ] 8.9 Add uninstaller registry entries: write to HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall
  - [ ] 8.10 Create installer UI sequence: use WixUI_InstallDir for custom install location or WixUI_Minimal for simple install
  - [ ] 8.11 Add license agreement: create `installer/license.rtf` with MIT or Apache 2.0 license text, reference in WiX
  - [ ] 8.12 Customize installer graphics: add `installer/banner.bmp` (493x58px) and `installer/dialog.bmp` (493x312px)
  - [ ] 8.13 Build MSI installer: run `candle.exe e01hasher.wxs` then `light.exe e01hasher.wixobj -out E01Hasher_v1.0.0.msi`
  - [ ] 8.14 Test installer on clean Windows 10 VM: verify installation succeeds, application launches, no DLL errors
  - [ ] 8.15 Test installer on clean Windows 11 VM: verify compatibility and functionality
  - [ ] 8.16 Test uninstaller: uninstall via Control Panel, verify all files removed, no registry remnants (optional: check with tools)
  - [ ] 8.17 Verify executable dependencies: run Dependency Walker on installed executable, confirm no external DLL dependencies except system libraries
  - [ ] 8.18 Code sign executable and MSI (optional but recommended): obtain code signing certificate, use signtool.exe to sign
  - [ ] 8.19 Create release notes in CHANGELOG.md: document v1.0.0 features, known limitations, installation instructions
  - [ ] 8.20 Package final release: MSI installer, README.md, LICENSE, CHANGELOG.md in release archive (ZIP) for distribution
