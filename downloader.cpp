#include "downloader.h"

#include "zippysharewebcontrol.h"

Downloader::Downloader(QObject *parent) : QObject(parent), isDownloading(false)
{

}

void Downloader::addDownloadList(QString zipDownloadUrl, QFileInfo fileInfo, std::function<void (qint64, qint64)> downloadProgressFunction)
{
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

void Downloader::remove(QString zipDownloadUrl)
{
    int index = zipDownloadUrlList.indexOf(zipDownloadUrl);
    if(index == -1){
        qDebug("Downloader::remove can't remove");
        return;
    }
    zipDownloadUrlList.removeAt(index);
    fileInfoList.removeAt(index);
}
