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

#endif // ZIPPYSHAREWEBCONTROL_H
