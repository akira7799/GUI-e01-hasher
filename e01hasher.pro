# E01 Hash Verification Tool
# Qt Project File (qmake)

# Application settings
TARGET = e01hasher
TEMPLATE = app

# Qt modules
QT += core gui widgets

# C++ standard (Qt 6 requires C++17 minimum)
CONFIG += c++17

# Static linking configuration
CONFIG += static
CONFIG += staticlib
DEFINES += STATIC

# Windows-specific configuration
win32 {
    # Application icon
    RC_ICONS = resources/icons/app_icon.ico

    # Windows version info
    VERSION = 1.0.0
    QMAKE_TARGET_COMPANY = "E01 Hash Verification Tool"
    QMAKE_TARGET_PRODUCT = "E01 Hash Verification Tool"
    QMAKE_TARGET_DESCRIPTION = "Forensic image hash verification utility"
    QMAKE_TARGET_COPYRIGHT = "Open Source"

    # Static linking flags for MinGW
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}

# Build directories
DESTDIR = build/release
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Include paths
INCLUDEPATH += $$PWD/src

# Platform-specific include paths
win32 {
    INCLUDEPATH += /tmp/libewf-install/include
}
unix {
    INCLUDEPATH += /usr/include
}

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

# Platform-specific library paths
win32 {
    LIBS += -L/tmp/libewf-install/lib
}

# libewf library
LIBS += -lewf

# Platform-specific crypto libraries
win32 {
    # Windows CryptoAPI
    LIBS += -ladvapi32
    LIBS += -lkernel32
    LIBS += -luser32
    LIBS += -lshell32
    LIBS += -luuid
    LIBS += -lole32
    LIBS += -loleaut32
    LIBS += -lnetapi32
    LIBS += -lws2_32
    LIBS += -lmpr
    LIBS += -lwinmm
    LIBS += -lversion
    LIBS += -luserenv
}

unix {
    # OpenSSL for Linux/Unix
    LIBS += -lcrypto
}

# Compiler warnings
QMAKE_CXXFLAGS += -Wall -Wextra

# Installation target (optional)
target.path = /usr/local/bin
INSTALLS += target

# Additional defines
DEFINES += QT_DEPRECATED_WARNINGS
