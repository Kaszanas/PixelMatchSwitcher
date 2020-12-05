#pragma once

#include <QObject>
#include <QByteArray>
#include <QtConcurrent/QtConcurrent>

#include <curl/curl.h>
#include <string>

#include "pm-structs.hpp"

/**
 * @brief Wrapper class for downloads using libcurl
 */
class PmFileRetriever : public QObject
{
    Q_OBJECT

public:
    PmFileRetriever(std::string fileUrl, QObject *parent = nullptr);
    ~PmFileRetriever();

    QFuture<CURLcode> startDownload(QThreadPool& threadPool);
    void abort() { m_abortFlag = 1; }

    void setSaveFilename(const std::string& saveFilename)
        { m_saveFilename = saveFilename; }

    QFuture<CURLcode>& future() { return m_future;}

    const QByteArray &data() const { return m_data; }

signals:
    void sigFailed(std::string urlName, QString error);
    void sigSucceeded(std::string urlName, QByteArray byteArray);
    void sigProgress(std::string urlName, size_t dlNow, size_t dlTotal);

protected slots:
    CURLcode onDownload();

protected:
    static int staticProgressFunc(void *clientp,
        curl_off_t dltotal, curl_off_t dlnow,
        curl_off_t ultotal, curl_off_t ulnow);
    static size_t staticWriteFunc(
        void *ptr, size_t size, size_t nmemb, void *data);

    void reset();

    std::string m_fileUrl;
    std::string m_saveFilename;
    QByteArray m_data;
    QFuture<CURLcode> m_future;

    CURL *m_curlHandle = nullptr;
    int m_abortFlag = 0;
};

/**
 * @brief Downloads preset XML from URL and associated images; 
 *        presents local presets
 */
class PmPresetsRetriever : public QObject
{
    Q_OBJECT

public:
    static const int k_numWorkerThreads = 4;

    PmPresetsRetriever(std::string xmlUrl);
    ~PmPresetsRetriever();

    void downloadXml();
    void retrievePresets(QList<std::string> selectedPresets);
    void downloadImages();

signals:
    // xml phase
    void sigXmlProgress(std::string xmlUrl, size_t dlNow, size_t dlTotal);
    void sigXmlFailed(std::string xmlUrl, QString error);
    void sigXmlPresetsAvailable(
        std::string xmlUrl, QList<std::string> presetNames);

    // image phase and results
    void sigImgProgress(std::string imgUrl, size_t dlNow, size_t dlTotal);
    void sigImgFailed(std::string imgUrl, QString error);
    void sigImgSuccess(std::string imgUrl);
    void sigPresetsReady(PmMatchPresets presets);
    void sigFailed();
    void sigAborted();

public slots:
    void onAbort();
    void onRetry();

protected slots:
    void onRetrievePresets();
    void onDownloadImages();
    void onDownloadXmlWorker();

protected:
    std::string m_xmlUrl;
    PmMatchPresets m_availablePresets;
    QList<std::string> m_selectedPresets;

    bool m_abort = false;
    QThreadPool m_workerThreadPool;
    PmFileRetriever* m_xmlRetriever;
    QList<PmFileRetriever*> m_imgRetrievers;
};
