/*
 * E01 Hash Verification Tool
 * HashEngine Implementation
 */

#include "hashengine.h"
#include <QDebug>
#include <QElapsedTimer>

HashEngine::HashEngine(EWFHandler *ewfHandler, QObject *parent)
    : QThread(parent)
    , ewfHandler(ewfHandler)
    , calculateMD5(true)
    , calculateSHA1(true)
    , calculateSHA256(true)
#ifdef _WIN32
    , hCryptProv(0)
    , hMD5(0)
    , hSHA1(0)
    , hSHA256(0)
#endif
    , cancelled(false)
{
}

HashEngine::~HashEngine()
{
    // Wait for thread to finish
    if (isRunning()) {
        cancel();
        wait();
    }
}

void HashEngine::enableMD5(bool enable)
{
    calculateMD5 = enable;
}

void HashEngine::enableSHA1(bool enable)
{
    calculateSHA1 = enable;
}

void HashEngine::enableSHA256(bool enable)
{
    calculateSHA256 = enable;
}

void HashEngine::setExpectedMD5(const QString &hash)
{
    expectedMD5 = hash.toLower().trimmed();
}

void HashEngine::setExpectedSHA1(const QString &hash)
{
    expectedSHA1 = hash.toLower().trimmed();
}

void HashEngine::setExpectedSHA256(const QString &hash)
{
    expectedSHA256 = hash.toLower().trimmed();
}

void HashEngine::cancel()
{
    cancelled = true;
}

void HashEngine::run()
{
    qDebug() << "HashEngine: Starting hash calculation";

    cancelled = false;

    // Verify EWF handler is open
    if (!ewfHandler || !ewfHandler->isOpen()) {
        emit error("File not open");
        return;
    }

    // Initialize hash contexts
    if (!initializeHashContexts()) {
        emit error("Failed to initialize hash contexts");
        cleanupHashContexts();
        return;
    }

    // Get total file size
    qint64 totalBytes = ewfHandler->getMediaSize();
    qint64 bytesProcessed = 0;

    qDebug() << "HashEngine: Processing" << totalBytes << "bytes";

    // Allocate read buffer
    char *buffer = new char[CHUNK_SIZE];
    if (!buffer) {
        emit error("Failed to allocate read buffer");
        cleanupHashContexts();
        return;
    }

    // Timer for progress updates
    QElapsedTimer timer;
    timer.start();
    qint64 lastProgressUpdate = 0;

    // Read and hash data in chunks
    while (bytesProcessed < totalBytes && !cancelled) {
        // Calculate how much to read
        qint64 bytesToRead = qMin(CHUNK_SIZE, totalBytes - bytesProcessed);

        // Read data
        qint64 bytesRead = ewfHandler->read(buffer, bytesToRead);

        if (bytesRead < 0) {
            emit error("Failed to read data from file");
            delete[] buffer;
            cleanupHashContexts();
            return;
        }

        if (bytesRead == 0) {
            // End of file
            break;
        }

        // Update hashes
        updateHashes(buffer, bytesRead);

        bytesProcessed += bytesRead;

        // Emit progress update every 100ms
        qint64 elapsed = timer.elapsed();
        if (elapsed - lastProgressUpdate >= 100) {
            calculateProgress(bytesProcessed, totalBytes);
            lastProgressUpdate = elapsed;
        }

        // Check for cancellation
        if (cancelled) {
            qDebug() << "HashEngine: Cancelled by user";
            delete[] buffer;
            cleanupHashContexts();
            return;
        }
    }

    delete[] buffer;

    // Final progress update
    calculateProgress(totalBytes, totalBytes);

    // Finalize hashes
    finalizeHashes();

    // Cleanup
    cleanupHashContexts();

    // Compare results and emit verification complete
    QMap<QString, bool> verificationResults;

    if (calculateMD5) {
        if (!expectedMD5.isEmpty()) {
            verificationResults["MD5"] = (calculatedMD5 == expectedMD5);
        } else {
            verificationResults["MD5"] = true;  // No expected hash, consider as pass
        }
    }

    if (calculateSHA1) {
        if (!expectedSHA1.isEmpty()) {
            verificationResults["SHA1"] = (calculatedSHA1 == expectedSHA1);
        } else {
            verificationResults["SHA1"] = true;
        }
    }

    if (calculateSHA256) {
        if (!expectedSHA256.isEmpty()) {
            verificationResults["SHA256"] = (calculatedSHA256 == expectedSHA256);
        } else {
            verificationResults["SHA256"] = true;
        }
    }

    emit verificationComplete(verificationResults);

    qDebug() << "HashEngine: Completed successfully";
}

bool HashEngine::initializeHashContexts()
{
#ifdef _WIN32
    // Acquire cryptographic provider
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        qDebug() << "HashEngine: Failed to acquire CryptoAPI context";
        return false;
    }

    // Create hash objects
    if (calculateMD5) {
        if (!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hMD5)) {
            qDebug() << "HashEngine: Failed to create MD5 hash";
            return false;
        }
    }

    if (calculateSHA1) {
        if (!CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hSHA1)) {
            qDebug() << "HashEngine: Failed to create SHA1 hash";
            return false;
        }
    }

    if (calculateSHA256) {
        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hSHA256)) {
            qDebug() << "HashEngine: Failed to create SHA256 hash";
            return false;
        }
    }
#else
    // Initialize OpenSSL hash contexts
    if (calculateMD5) {
        MD5_Init(&md5Context);
    }

    if (calculateSHA1) {
        SHA1_Init(&sha1Context);
    }

    if (calculateSHA256) {
        SHA256_Init(&sha256Context);
    }
#endif

    return true;
}

void HashEngine::updateHashes(const char *data, qint64 size)
{
#ifdef _WIN32
    const BYTE *byteData = reinterpret_cast<const BYTE*>(data);
    DWORD dataSize = static_cast<DWORD>(size);

    if (calculateMD5 && hMD5) {
        CryptHashData(hMD5, byteData, dataSize, 0);
    }

    if (calculateSHA1 && hSHA1) {
        CryptHashData(hSHA1, byteData, dataSize, 0);
    }

    if (calculateSHA256 && hSHA256) {
        CryptHashData(hSHA256, byteData, dataSize, 0);
    }
#else
    const unsigned char *byteData = reinterpret_cast<const unsigned char*>(data);

    if (calculateMD5) {
        MD5_Update(&md5Context, byteData, size);
    }

    if (calculateSHA1) {
        SHA1_Update(&sha1Context, byteData, size);
    }

    if (calculateSHA256) {
        SHA256_Update(&sha256Context, byteData, size);
    }
#endif
}

void HashEngine::finalizeHashes()
{
#ifdef _WIN32
    BYTE hashBuffer[256];  // Large enough for any hash
    DWORD hashSize;

    // Finalize MD5
    if (calculateMD5 && hMD5) {
        hashSize = 16;  // MD5 is 16 bytes
        if (CryptGetHashParam(hMD5, HP_HASHVAL, hashBuffer, &hashSize, 0)) {
            calculatedMD5 = hashToHexString(hashBuffer, hashSize);
            emit md5Calculated(calculatedMD5);
            qDebug() << "MD5:" << calculatedMD5;
        }
    }

    // Finalize SHA1
    if (calculateSHA1 && hSHA1) {
        hashSize = 20;  // SHA1 is 20 bytes
        if (CryptGetHashParam(hSHA1, HP_HASHVAL, hashBuffer, &hashSize, 0)) {
            calculatedSHA1 = hashToHexString(hashBuffer, hashSize);
            emit sha1Calculated(calculatedSHA1);
            qDebug() << "SHA1:" << calculatedSHA1;
        }
    }

    // Finalize SHA256
    if (calculateSHA256 && hSHA256) {
        hashSize = 32;  // SHA256 is 32 bytes
        if (CryptGetHashParam(hSHA256, HP_HASHVAL, hashBuffer, &hashSize, 0)) {
            calculatedSHA256 = hashToHexString(hashBuffer, hashSize);
            emit sha256Calculated(calculatedSHA256);
            qDebug() << "SHA256:" << calculatedSHA256;
        }
    }
#else
    unsigned char hashBuffer[256];  // Large enough for any hash

    // Finalize MD5
    if (calculateMD5) {
        MD5_Final(hashBuffer, &md5Context);
        calculatedMD5 = hashToHexString(hashBuffer, MD5_DIGEST_LENGTH);
        emit md5Calculated(calculatedMD5);
        qDebug() << "MD5:" << calculatedMD5;
    }

    // Finalize SHA1
    if (calculateSHA1) {
        SHA1_Final(hashBuffer, &sha1Context);
        calculatedSHA1 = hashToHexString(hashBuffer, SHA_DIGEST_LENGTH);
        emit sha1Calculated(calculatedSHA1);
        qDebug() << "SHA1:" << calculatedSHA1;
    }

    // Finalize SHA256
    if (calculateSHA256) {
        SHA256_Final(hashBuffer, &sha256Context);
        calculatedSHA256 = hashToHexString(hashBuffer, SHA256_DIGEST_LENGTH);
        emit sha256Calculated(calculatedSHA256);
        qDebug() << "SHA256:" << calculatedSHA256;
    }
#endif
}

void HashEngine::cleanupHashContexts()
{
#ifdef _WIN32
    if (hMD5) {
        CryptDestroyHash(hMD5);
        hMD5 = 0;
    }

    if (hSHA1) {
        CryptDestroyHash(hSHA1);
        hSHA1 = 0;
    }

    if (hSHA256) {
        CryptDestroyHash(hSHA256);
        hSHA256 = 0;
    }

    if (hCryptProv) {
        CryptReleaseContext(hCryptProv, 0);
        hCryptProv = 0;
    }
#else
    // OpenSSL contexts are stack-allocated, no cleanup needed
#endif
}

#ifdef _WIN32
QString HashEngine::hashToHexString(const BYTE *hash, DWORD hashSize)
#else
QString HashEngine::hashToHexString(const unsigned char *hash, unsigned int hashSize)
#endif
{
    QString hexString;
    hexString.reserve(hashSize * 2);

    for (unsigned int i = 0; i < hashSize; ++i) {
        hexString += QString("%1").arg(hash[i], 2, 16, QChar('0'));
    }

    return hexString.toLower();
}

void HashEngine::calculateProgress(qint64 bytesProcessed, qint64 totalBytes)
{
    int percentage = 0;
    if (totalBytes > 0) {
        percentage = static_cast<int>((bytesProcessed * 100) / totalBytes);
    }

    emit progressUpdate(percentage, bytesProcessed, totalBytes);

    // Calculate time remaining
    // This is a simplified estimation - could be improved with moving average
    static QElapsedTimer overallTimer;
    static bool timerStarted = false;

    if (!timerStarted) {
        overallTimer.start();
        timerStarted = true;
    }

    if (percentage > 0 && percentage < 100) {
        qint64 elapsedMs = overallTimer.elapsed();
        qint64 totalEstimatedMs = (elapsedMs * 100) / percentage;
        qint64 remainingMs = totalEstimatedMs - elapsedMs;

        int remainingSeconds = static_cast<int>(remainingMs / 1000);
        int minutes = remainingSeconds / 60;
        int seconds = remainingSeconds % 60;

        QString timeString;
        if (minutes > 0) {
            timeString = QString("%1m %2s").arg(minutes).arg(seconds);
        } else {
            timeString = QString("%1s").arg(seconds);
        }

        emit timeEstimate(timeString);
    } else if (percentage >= 100) {
        timerStarted = false;
    }
}
