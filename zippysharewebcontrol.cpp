#include "zippysharewebcontrol.h"

#include <QJsonObject>
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QUrl>
#include <QFileDialog>

#include "webautomaticindexer.h"
#include "commonfunction.h"

ZippyshareWebControl::ZippyshareWebControl(QString zipUrl, QObject *parent) : EasyWebControl(parent), zipUrl(zipUrl)
{
    connect(web, &WebAutomaticIndexer::downloadProgress, this, &ZippyshareWebControl::downloadProgress);
    /*connect(web, &WebAutomaticIndexer::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){
        QString sentUnit = "b", totalUnit = "b";
        if(bytesReceived / 1024 > 0){
            bytesReceived /= 1024;
            sentUnit = "kb";
        }
        if(bytesReceived / 1024 > 0){
            bytesReceived /= 1024;
            sentUnit = "mb";
        }
        if(bytesTotal / 1024 > 0){
            bytesTotal /= 1024;
            totalUnit = "kb";
        }
        if(bytesTotal / 1024 > 0){
            bytesTotal /= 1024;
            totalUnit = "mb";
        }
        qDebug() << bytesReceived * 100 / bytesTotal << bytesReceived << sentUnit.toLocal8Bit().data() << bytesTotal << totalUnit.toLocal8Bit().data();
    });*/
}

/*ZippyshareWebControl::ZippyshareWebControl(AnimeDownloadInfo info, QObject *parent) : EasyWebControl(parent), info(info)
{
    connect(web, &WebAutomaticIndexer::downloadProgress, this, &ZippyshareWebControl::downloadProgress);

    downloadUrl = info.url;
}*/

void ZippyshareWebControl::download(QFileInfo fileInfo)
{
    getHtml();

    QString path = fileInfo.path();
    //path = path.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");

    if(!QDir(path).exists()){
        QDir().mkdir(path);
        qDebug() << "ZippyshareWebControl::download create dir" << path;
    }

    QString mainWeb = capturedString(zipUrl, "(http://.+?)/");

    QString sourcelink = capturedString(zipHtml, ".href = (.+?);");
    QString linkPart1 = capturedString(sourcelink, "\"(.+?)\"");
    QString linkPart2 = capturedString(sourcelink, "\\((.+?)\\)");
    QString linkPart3 = capturedString(sourcelink.remove(0, linkPart1.size()+2), "\"(.+?)\"");
    QString downloadLink = mainWeb + linkPart1 + QString::number(calculator(linkPart2)) + linkPart3;
    //qDebug() << downloadLink;

    QByteArray data = web->get(downloadLink);

    QFile file(fileInfo.filePath());
    if(file.open(QFile::WriteOnly)){
        file.write(data);
        file.close();
    }
}

void ZippyshareWebControl::download(QFileInfo fileInfo, qint64 startByte, qint64 endByte)
{
    getHtml();

    QString path = fileInfo.path();

    if(!QDir(path).exists()){
        QDir().mkdir(path);
        qDebug() << "ZippyshareWebControl::download create dir" << path;
    }

    QString mainWeb = capturedString(zipUrl, "(http://.+?)/");

    QString sourcelink = capturedString(zipHtml, ".href = (.+?);");
    QString linkPart1 = capturedString(sourcelink, "\"(.+?)\"");
    QString linkPart2 = capturedString(sourcelink, "\\((.+?)\\)");
    QString linkPart3 = capturedString(sourcelink.remove(0, linkPart1.size()+2), "\"(.+?)\"");
    QString downloadLink = mainWeb + linkPart1 + QString::number(calculator(linkPart2)) + linkPart3;

    QStringList header;
    header << "Range" << QString::asprintf("bytes=%d-%d", startByte, endByte);

    QByteArray data = web->get(downloadLink, header);
    qDebug() << web->getLastStatusCode();
    qDebug() << web->getLastReplyHeaderList();
    qDebug() << web->getLastReplyHeaderValueList();

    QFile file(fileInfo.filePath());
    if(file.open(QFile::WriteOnly)){
        file.write(data);
        file.close();
    }
}

QString ZippyshareWebControl::getTitle()
{
    if(title.isNull()){
        getHtml();

        QString sourcelink = capturedString(zipHtml, ".href = (.+?);");
        QString linkPart1 = capturedString(sourcelink, "\"(.+?)\"");
        QString linkPart3 = capturedString(sourcelink.remove(0, linkPart1.size()+2), "\"(.+?)\"");
        title = QUrl::fromPercentEncoding(codec->fromUnicode(linkPart3.remove(0,1)));
    }

    return title;
}

void ZippyshareWebControl::getTitle2(std::function<void (QString)> f, QString zipUrl)
{
    ZippyshareWebControl *z = new ZippyshareWebControl(zipUrl);
    z->web->get2(zipUrl, QStringList(), [=](QByteArray html){
        z->zipHtml = z->codec->toUnicode(html);
        f(z->getTitle());
        z->deleteLater();
    });
}


int ZippyshareWebControl::calculator(QString str)
{
    QStringList strList = str.split(' ');

    if(strList.size() == 7){
        if(strList.at(1) == "%" && strList.at(3) == "+" && strList.at(5) == "%"){
            return ((strList.at(0).toInt() % strList.at(2).toInt()) + (strList.at(4).toInt() % strList.at(6).toInt()));
        }
    }
    qWarning() << "ZippyshareWebControl::calculator str=" << str;

    return 0;
}

void ZippyshareWebControl::getHtml()
{
    if(zipHtml.isEmpty()){
        if(zipUrl.isEmpty()){
            qWarning() << "ZippyshareWebControl::getDownloadUrl zipUrl is empty";
            return;
        }
        QByteArray html = web->get(zipUrl);
        zipHtml = codec->toUnicode(html);
    }
}
