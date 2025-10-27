/*
 * E01 Hash Verification Tool
 * HashEngine - Background hash calculation thread
 */

#ifndef HASHENGINE_H
#define HASHENGINE_H

#include <QThread>
#include <QString>
#include <QMap>
#include "ewfhandler.h"

// Windows CryptoAPI headers
#include <windows.h>
#include <wincrypt.h>

class HashEngine : public QThread
{
    Q_OBJECT

public:
    explicit HashEngine(EWFHandler *ewfHandler, QObject *parent = nullptr);
    ~HashEngine();

    // Hash algorithm selection
    void enableMD5(bool enable);
    void enableSHA1(bool enable);
    void enableSHA256(bool enable);

    // Expected hashes for verification
    void setExpectedMD5(const QString &hash);
    void setExpectedSHA1(const QString &hash);
    void setExpectedSHA256(const QString &hash);

    // Control
    void cancel();

signals:
    // Progress signals
    void progressUpdate(int percentage, qint64 bytesProcessed, qint64 totalBytes);
    void timeEstimate(const QString &remaining);

    // Hash results
    void md5Calculated(const QString &hash);
    void sha1Calculated(const QString &hash);
    void sha256Calculated(const QString &hash);

    // Verification results
    void verificationComplete(const QMap<QString, bool> &results);

    // Error signal
    void error(const QString &errorMessage);

protected:
    void run() override;

private:
    // Hash calculation
    bool initializeHashContexts();
    void updateHashes(const char *data, qint64 size);
    void finalizeHashes();
    void cleanupHashContexts();

    // Helper functions
    QString hashToHexString(const BYTE *hash, DWORD hashSize);
    void calculateProgress(qint64 bytesProcessed, qint64 totalBytes);

    // EWF handler
    EWFHandler *ewfHandler;

    // Hash algorithm selection
    bool calculateMD5;
    bool calculateSHA1;
    bool calculateSHA256;

    // Expected hashes
    QString expectedMD5;
    QString expectedSHA1;
    QString expectedSHA256;

    // Calculated hashes
    QString calculatedMD5;
    QString calculatedSHA1;
    QString calculatedSHA256;

    // Windows CryptoAPI handles
    HCRYPTPROV hCryptProv;
    HCRYPTHASH hMD5;
    HCRYPTHASH hSHA1;
    HCRYPTHASH hSHA256;

    // Control flags
    bool cancelled;

    // Constants
    static const qint64 CHUNK_SIZE = 1024 * 1024;  // 1MB chunks
};

#endif // HASHENGINE_H
