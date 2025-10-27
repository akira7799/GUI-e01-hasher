/*
 * E01 Hash Verification Tool
 * EWFHandler Implementation
 */

#include "ewfhandler.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <cstring>

EWFHandler::EWFHandler()
    : handle(nullptr)
    , error(nullptr)
    , opened(false)
    , mediaSize(0)
    , metadataCached(false)
{
}

EWFHandler::~EWFHandler()
{
    close();
}

bool EWFHandler::open(const QString &filePath)
{
    // Close any previously opened file
    if (opened) {
        close();
    }

    // Check if file exists
    if (!QFile::exists(filePath)) {
        setError("File does not exist: " + filePath);
        return false;
    }

    // Initialize libewf handle
    if (libewf_handle_initialize(&handle, &error) != 1) {
        setError("Failed to initialize libewf handle");
        return false;
    }

    // Detect and glob segments
    char **filenames = nullptr;
    int fileCount = 0;

    if (!detectAndGlobSegments(filePath, &filenames, &fileCount)) {
        libewf_handle_free(&handle, nullptr);
        handle = nullptr;
        return false;
    }

    // Open the file(s) with libewf
    int result = libewf_handle_open(
        handle,
        filenames,
        fileCount,
        LIBEWF_OPEN_READ,
        &error
    );

    // Free the globbed filenames
    if (filenames != nullptr) {
        libewf_glob_free(filenames, fileCount, nullptr);
    }

    if (result != 1) {
        setError("Failed to open E01 file with libewf");
        libewf_handle_free(&handle, nullptr);
        handle = nullptr;
        return false;
    }

    // Get media size
    size64_t size = 0;
    if (libewf_handle_get_media_size(handle, &size, &error) != 1) {
        setError("Failed to get media size");
        libewf_handle_close(handle, nullptr);
        libewf_handle_free(&handle, nullptr);
        handle = nullptr;
        return false;
    }

    mediaSize = static_cast<qint64>(size);
    currentFilePath = filePath;
    opened = true;
    lastError.clear();

    return true;
}

void EWFHandler::close()
{
    if (handle != nullptr) {
        libewf_handle_close(handle, nullptr);
        libewf_handle_free(&handle, nullptr);
        handle = nullptr;
    }

    opened = false;
    currentFilePath.clear();
    mediaSize = 0;
    cachedMetadata.clear();
    metadataCached = false;
}

bool EWFHandler::isOpen() const
{
    return opened;
}

qint64 EWFHandler::read(char *buffer, qint64 maxSize)
{
    if (!opened || handle == nullptr) {
        setError("File not open");
        return -1;
    }

    ssize_t bytesRead = libewf_handle_read_buffer(
        handle,
        buffer,
        static_cast<size_t>(maxSize),
        &error
    );

    if (bytesRead < 0) {
        setError("Failed to read data from file");
        return -1;
    }

    return static_cast<qint64>(bytesRead);
}

qint64 EWFHandler::readAt(char *buffer, qint64 maxSize, qint64 offset)
{
    if (!opened || handle == nullptr) {
        setError("File not open");
        return -1;
    }

    ssize_t bytesRead = libewf_handle_read_buffer_at_offset(
        handle,
        buffer,
        static_cast<size_t>(maxSize),
        static_cast<off64_t>(offset),
        &error
    );

    if (bytesRead < 0) {
        setError("Failed to read data at offset");
        return -1;
    }

    return static_cast<qint64>(bytesRead);
}

qint64 EWFHandler::getMediaSize() const
{
    return mediaSize;
}

QString EWFHandler::getFilePath() const
{
    return currentFilePath;
}

QMap<QString, QString> EWFHandler::getMetadata()
{
    if (!opened || handle == nullptr) {
        return QMap<QString, QString>();
    }

    // Return cached metadata if available
    if (metadataCached) {
        return cachedMetadata;
    }

    // Extract metadata
    cachedMetadata.clear();

    cachedMetadata["file_path"] = currentFilePath;
    cachedMetadata["media_size"] = QString::number(mediaSize);

    // Extract header values
    cachedMetadata["case_number"] = getHeaderValue("case_number");
    cachedMetadata["description"] = getHeaderValue("description");
    cachedMetadata["examiner_name"] = getHeaderValue("examiner_name");
    cachedMetadata["evidence_number"] = getHeaderValue("evidence_number");
    cachedMetadata["notes"] = getHeaderValue("notes");
    cachedMetadata["acquiry_date"] = getHeaderValue("acquiry_date");
    cachedMetadata["system_date"] = getHeaderValue("system_date");

    // Extract stored hashes
    QString md5 = getStoredMD5();
    QString sha1 = getStoredSHA1();

    if (!md5.isEmpty()) {
        cachedMetadata["stored_md5"] = md5;
    }
    if (!sha1.isEmpty()) {
        cachedMetadata["stored_sha1"] = sha1;
    }

    metadataCached = true;
    return cachedMetadata;
}

QString EWFHandler::getMetadataValue(const QString &key)
{
    if (!metadataCached) {
        getMetadata();
    }

    return cachedMetadata.value(key, QString());
}

QString EWFHandler::getStoredMD5()
{
    if (!opened || handle == nullptr) {
        return QString();
    }

    return getHashValue("MD5");
}

QString EWFHandler::getStoredSHA1()
{
    if (!opened || handle == nullptr) {
        return QString();
    }

    return getHashValue("SHA1");
}

bool EWFHandler::hasStoredHash(const QString &algorithm)
{
    QString hash;

    if (algorithm.toUpper() == "MD5") {
        hash = getStoredMD5();
    } else if (algorithm.toUpper() == "SHA1" || algorithm.toUpper() == "SHA-1") {
        hash = getStoredSHA1();
    }

    return !hash.isEmpty();
}

QString EWFHandler::getLastError() const
{
    return lastError;
}

// ===== Private Helper Functions =====

bool EWFHandler::detectAndGlobSegments(const QString &filePath, char ***filenames, int *fileCount)
{
    QByteArray filePathBytes = filePath.toLocal8Bit();
    const char *path = filePathBytes.constData();

    // Use libewf_glob to detect all segments
    int result = libewf_glob(
        path,
        strlen(path),
        LIBEWF_FORMAT_UNKNOWN,
        filenames,
        fileCount,
        &error
    );

    if (result != 1) {
        // Glob failed, try opening the single file
        qDebug() << "libewf_glob failed, attempting single file open";

        // Allocate array for single filename
        *filenames = static_cast<char**>(malloc(sizeof(char*)));
        if (*filenames == nullptr) {
            setError("Failed to allocate memory for filename");
            return false;
        }

        // Duplicate the filename
        (*filenames)[0] = strdup(path);
        if ((*filenames)[0] == nullptr) {
            free(*filenames);
            setError("Failed to duplicate filename");
            return false;
        }

        *fileCount = 1;
        return true;
    }

    if (*fileCount == 0) {
        setError("No segments found for file: " + filePath);
        return false;
    }

    qDebug() << "Found" << *fileCount << "segment(s) for" << filePath;
    return true;
}

QString EWFHandler::getHeaderValue(const char *identifier)
{
    if (!opened || handle == nullptr) {
        return QString();
    }

    size_t valueSize = 0;
    uint8_t *value = nullptr;

    // Get the size of the value
    int result = libewf_handle_get_utf8_header_value(
        handle,
        reinterpret_cast<const uint8_t*>(identifier),
        strlen(identifier),
        nullptr,
        0,
        &error
    );

    if (result != 1) {
        return QString();
    }

    // Get the actual size
    result = libewf_handle_get_utf8_header_value_size(
        handle,
        reinterpret_cast<const uint8_t*>(identifier),
        strlen(identifier),
        &valueSize,
        &error
    );

    if (result != 1 || valueSize == 0) {
        return QString();
    }

    // Allocate buffer and get the value
    value = new uint8_t[valueSize];
    result = libewf_handle_get_utf8_header_value(
        handle,
        reinterpret_cast<const uint8_t*>(identifier),
        strlen(identifier),
        value,
        valueSize,
        &error
    );

    QString strValue;
    if (result == 1) {
        strValue = QString::fromUtf8(reinterpret_cast<const char*>(value));
    }

    delete[] value;
    return strValue;
}

QString EWFHandler::getHashValue(const char *identifier)
{
    if (!opened || handle == nullptr) {
        return QString();
    }

    size_t valueSize = 0;
    uint8_t *value = nullptr;

    // Get the size of the hash value
    int result = libewf_handle_get_utf8_hash_value_size(
        handle,
        reinterpret_cast<const uint8_t*>(identifier),
        strlen(identifier),
        &valueSize,
        &error
    );

    if (result != 1 || valueSize == 0) {
        return QString();
    }

    // Allocate buffer and get the value
    value = new uint8_t[valueSize];
    result = libewf_handle_get_utf8_hash_value(
        handle,
        reinterpret_cast<const uint8_t*>(identifier),
        strlen(identifier),
        value,
        valueSize,
        &error
    );

    QString hashValue;
    if (result == 1) {
        hashValue = QString::fromUtf8(reinterpret_cast<const char*>(value));
        hashValue = hashValue.trimmed().toLower();  // Normalize hash format
    }

    delete[] value;
    return hashValue;
}

void EWFHandler::setError(const QString &errorMsg)
{
    lastError = errorMsg;
    qDebug() << "EWFHandler Error:" << errorMsg;
}
