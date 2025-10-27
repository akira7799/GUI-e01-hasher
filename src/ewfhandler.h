/*
 * E01 Hash Verification Tool
 * EWFHandler - libewf C API wrapper
 */

#ifndef EWFHANDLER_H
#define EWFHANDLER_H

#include <QString>
#include <QMap>
#include <libewf.h>

class EWFHandler
{
public:
    EWFHandler();
    ~EWFHandler();

    // File operations
    bool open(const QString &filePath);
    void close();
    bool isOpen() const;

    // Data reading
    qint64 read(char *buffer, qint64 maxSize);
    qint64 readAt(char *buffer, qint64 maxSize, qint64 offset);

    // File information
    qint64 getMediaSize() const;
    QString getFilePath() const;

    // Metadata extraction
    QMap<QString, QString> getMetadata();
    QString getMetadataValue(const QString &key);

    // Stored hash values from E01 file
    QString getStoredMD5();
    QString getStoredSHA1();
    bool hasStoredHash(const QString &algorithm);

    // Error handling
    QString getLastError() const;

private:
    // Helper functions
    bool detectAndGlobSegments(const QString &filePath, char ***filenames, int *fileCount);
    QString getHeaderValue(const char *identifier);
    QString getHashValue(const char *identifier);
    void setError(const QString &errorMsg);

    // libewf handle
    libewf_handle_t *handle;
    libewf_error_t *error;

    // State
    bool opened;
    QString currentFilePath;
    QString lastError;
    qint64 mediaSize;

    // Cached metadata
    QMap<QString, QString> cachedMetadata;
    bool metadataCached;
};

#endif // EWFHANDLER_H
