#pragma once

#include <QObject>

#include <curl/curl.h>
#include <string>
#include "pm-structs.hpp"

typedef int PmFileRetrieverProgressFunc(void *clientp,
    curl_off_t dltotal, curl_off_t dlnow,
    curl_off_t ultotal, curl_off_t ulnow); 

typedef size_t PmFileRetrieverWriteFunc(
    void *ptr, size_t size, size_t nmemb, void *stream);

class PmFileRetriever : public QObject
{
    Q_OBJECT

public:
    static const int k_numRetries = 5;
    static const int k_msBeforeRetry = 3000;

    enum FileRetrieverState {
        Idle, Downloading, RetryPending, Halted, Done, Failed };

    PmFileRetriever(QString fileUrl, QObject *parent = nullptr);
    ~PmFileRetriever();

    FileRetrieverState state() const { return m_state; }

    void startDownload();
    void halt();

signals:
    void sigFailed(QString urlName, QString error);
    void sigSucceeded(QString urlName, QByteArray byteArray);
    void sigProgress(QString urlName, size_t dlNow, size_t dlTotal);

protected:
    static int staticProgressFunc(void *clientp,
        curl_off_t dltotal, curl_off_t dlnow,
        curl_off_t ultotal, curl_off_t ulnow);
    static size_t staticWriteFunc(
        void *ptr, size_t size, size_t nmemb, void *data);

    void reset();

    QString m_fileUrl;
    QByteArray m_data;

    FileRetrieverState m_state = Idle;
    CURL *m_curlHandle = nullptr;

    int retriesLeft = k_numRetries;
    int msLeftBeforeRetry = k_msBeforeRetry;
};

class PmPresetRetriever : public QObject
{
    Q_OBJECT

public:
    static const int k_numConcurrentDownloads = 3;
    enum PresetRetrieverState
        { Idle, DownloadingXML, MakeSelection, DownloadingImages };

    PmPresetRetriever(QString xmlUrl, QObject *parent = nullptr);

signals:
    // xml download
    QList<QString> xmlPresetsAvailable();
    void sigXmlFailed(QString error);

public slots:
    void downloadPresets(QList<QString> presetName);

protected slots:
    // xml download
    void onXmlProgress(QString xmlUrl, size_t dlNow, size_t dlTotal);
    void onXmlFailed(QString xmlUrl, QString error);
    void onXmlSucceeded(QString xmlUrl, QByteArray data);

    // images download
    void onImageProgress(QString imageFilename, int percent);
    void onImageFailed(QString , QString error);

protected:
    QString m_xmlUrl;
    int m_numActiveDownloads = 0;
    PresetRetrieverState m_state = Idle;
    QThread *m_thread;
};
