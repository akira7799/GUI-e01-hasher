# E01 Hash Verification Tool
# Qt Project File for Windows Build

TARGET = e01hasher
TEMPLATE = app

# Qt modules
QT += core gui widgets

# C++ standard (Qt 6 requires C++17 minimum)
CONFIG += c++17

# Release configuration
CONFIG += release

# Windows-specific configuration
win32 {
    # Application icon (uncomment when icon file is available)
    # RC_ICONS = resources/icons/app_icon.ico

    # Windows version info
    VERSION = 1.0.0
    QMAKE_TARGET_COMPANY = "E01 Hash Verification Tool"
    QMAKE_TARGET_PRODUCT = "E01 Hash Verification Tool"
    QMAKE_TARGET_DESCRIPTION = "Forensic image hash verification utility"
    QMAKE_TARGET_COPYRIGHT = "Open Source"

    # Link flags for MinGW
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++
}

# Build directories
DESTDIR = release
OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rcc
UI_DIR = ui

# Include paths
INCLUDEPATH += $$PWD/src

# Source files
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ewfhandler.cpp \
    src/hashengine.cpp

# Header files
HEADERS += \
    src/mainwindow.h \
    src/ewfhandler.h \
    src/hashengine.h

# UI files
FORMS +=

# Resource files
RESOURCES += resources/application.qrc

# ===== LibEWF Configuration =====
# LibEWF path (Windows accessible location)
LIBEWF_DIR = C:/Users/dave/Desktop/Coding/ClaudeCode/libewf-install

# Check if LIBEWF_DIR is defined
!isEmpty(LIBEWF_DIR) {
    message("Using libewf from: $$LIBEWF_DIR")
    INCLUDEPATH += $$LIBEWF_DIR/include

    # Link libewf and all its dependencies in correct order
    LIBS += -L$$LIBEWF_DIR/lib
    LIBS += -lewf -lbfio -lodraw -lsmdev -lsmraw
    LIBS += -lhmac -lcaes
    LIBS += -lfvalue -lfdata -lfcache
    LIBS += -lfdatetime -lcdatetime -lfguid
    LIBS += -lcdata -lcthreads -lcerror -lcnotify
    LIBS += -lcfile -lcpath -lclocale -lcsplit -luna
} else {
    warning("LIBEWF_DIR not set - you need to configure libewf path")
    warning("Set environment variable: LIBEWF_DIR=C:/path/to/libewf-install")
}

# Windows CryptoAPI (for MD5, SHA1, SHA256)
win32 {
    LIBS += -ladvapi32
    LIBS += -lkernel32
    LIBS += -luser32
    LIBS += -lshell32
}

# Compiler warnings
QMAKE_CXXFLAGS += -Wall -Wextra

# Additional defines
DEFINES += QT_DEPRECATED_WARNINGS
