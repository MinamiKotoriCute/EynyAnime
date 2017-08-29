#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <functional>

#include <QObject>
#include <QFileInfo>

class Downloader : public QObject
{
    Q_OBJECT

public:
    Downloader(QObject *parent = 0);

    void addDownloadList(QString zipDownloadUrl, QFileInfo fileInfo, std::function<void (qint64 bytesReceived, qint64 bytesTotal)> downloadProgressFunction);

    void remove(QString zipDownloadUrl);

signals:
    void oneFileFinish();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QStringList zipDownloadUrlList;
    QList<QFileInfo> fileInfoList;
    QList<std::function<void (qint64 bytesReceived, qint64 bytesTotal)>> downloadProgressFunctionList;
    bool isDownloading;
};

#endif // DOWNLOADER_H
