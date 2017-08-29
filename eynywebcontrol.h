#ifndef EYNYWEBCONTROL_H
#define EYNYWEBCONTROL_H

#include <functional>

#include <QObject>
#include <QTextCodec>
#include <QImage>

#include <easywebcontrol.h>

class EynyWebControl : public EasyWebControl
{
    Q_OBJECT
public:
    explicit EynyWebControl(QObject *parent = 0);

    void login(QString username, QString password);
    void login(QString username, QString password, std::function<void (void)> f);
    QJsonObject allAnimePageInfo(int needPage = -1);
    QImage getImage(QString url);
    void getImage(QString url, std::function<void (QImage)> f);

    template<typename F>
    void getImage2(QString url, F function);
    int getAnimeTotalPage();

    QJsonObject aglaia123Analysis(QString url);
    QJsonObject aglaia123Analysis2(QString url);
    void aglaia123Analysis2(QString url, std::function<void (QJsonObject)> f);
    void aglaia123Analysis3(QString url, std::function<void (QString /*title*/, QStringList /*downloadUrls*/)> f);

signals:
    void updateInformation(int);

public slots:

private:
    void home();
    QJsonObject getLoginInfo();
    void getLoginInfo(std::function<void (QJsonObject)> f);
    QJsonObject animeBlock(QString block);

};

template<typename F>
void EynyWebControl::getImage2(QString url, F function)
{
    url.replace("/./", "/");

    QStringList header;
    header << "Referer" << "http://www02.eyny.com/forum-22-1.html";

    web->get2(url, header, [=](QByteArray b){
        function(QImage::fromData(b));
    });
}

#endif // EYNYWEBCONTROL_H
