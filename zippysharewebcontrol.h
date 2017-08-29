#ifndef ZIPPYSHAREWEBCONTROL_H
#define ZIPPYSHAREWEBCONTROL_H

#include <functional>

#include <QObject>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QFileInfo>

#include "easywebcontrol.h"
#include "animeinfo.h"

class ZippyshareWebControl : public EasyWebControl
{
    Q_OBJECT

public:
    explicit ZippyshareWebControl(QString zipUrl, QObject *parent = 0);

    void download(QFileInfo fileInfo);
    void download(QFileInfo fileInfo, qint64 startByte, qint64 endByte);
    QString getTitle();
    static void getTitle2(std::function<void (QString)> f, QString zipUrl);


signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    int calculator(QString str);
    void getHtml();

    QString zipUrl;
    QString zipHtml;
    QString title;
};

class Downloader : public QObject
{
    Q_OBJECT

public:
    Downloader(QObject *parent = 0) : QObject(parent), isDownloading(false){}

    void addDownloadList(QString zipDownloadUrl, QFileInfo fileInfo, std::function<void (qint64 bytesReceived, qint64 bytesTotal)> downloadProgressFunction){
        zipDownloadUrlList << zipDownloadUrl;
        fileInfoList << fileInfo;
        downloadProgressFunctionList << downloadProgressFunction;

        if(!isDownloading){
            isDownloading = true;
            while(zipDownloadUrlList.size() > 0){
                ZippyshareWebControl z(zipDownloadUrlList.takeFirst());
                //connect(&z, &ZippyshareWebControl::downloadProgress, this, &Downloader::downloadProgress);
                connect(&z, &ZippyshareWebControl::downloadProgress, downloadProgressFunctionList.takeFirst());

                z.download(fileInfoList.takeFirst());
                emit oneFileFinish();
            }
            isDownloading = false;
        }
    }

    void remove(QString zipDownloadUrl){
        int index = zipDownloadUrlList.indexOf(zipDownloadUrl);
        if(index == -1){
            qDebug("Downloader::remove can't remove");
            return;
        }
        zipDownloadUrlList.removeAt(index);
        fileInfoList.removeAt(index);
    }

signals:
    void oneFileFinish();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QStringList zipDownloadUrlList;
    QList<QFileInfo> fileInfoList;
    QList<std::function<void (qint64 bytesReceived, qint64 bytesTotal)>> downloadProgressFunctionList;
    bool isDownloading;
};

#endif // ZIPPYSHAREWEBCONTROL_H
