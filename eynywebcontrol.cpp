#include "eynywebcontrol.h"

#include <QJsonObject>
#include <QDebug>
#include <QRegularExpression>
#include <QImage>
#include <QFile>
#include <QJsonArray>
#include <QTime>

#include "zippysharewebcontrol.h"
#include "webautomaticindexer.h"
#include "commonfunction.h"
#include "asynchronous.h"

void saveImage(QByteArray b){
    static int imgCount = 0;
    QFile file(QString::asprintf("img%d.jpg", ++imgCount));
    if(file.open(QFile::WriteOnly)){
        file.write(b);
        file.close();
    }
}

EynyWebControl::EynyWebControl(QObject *parent) : EasyWebControl(parent)
{
    //getImage("http://a439.file-static.com/data/attachment/forum/picture/1704/11310725.jpg").save("QQ.jpg");

    /*home();
    login("joeyten2000", "sophie10");
    saveJson(allAnimePageInfo());

    QJsonObject allAnimeInfo = loadJson();
    QList<QJsonObject> canAnalysisAnimeList;

    for(QString key : allAnimeInfo.keys()){
        QJsonObject animeInfo = allAnimeInfo.value(key).toObject();
        if(animeInfo["author"].toString() == "aglaia123"){
            canAnalysisAnimeList.push_back(animeInfo);
            static int aasd = 0;
            //qDebug() << ++aasd << animeInfo["title"];
        }
    }

    QJsonObject downloadInfo = aglaia123Analysis("http://www02.eyny.com/" + canAnalysisAnimeList.at(2)["link"].toString());
    qDebug() << downloadInfo;*/

    /*QByteArray html = web->get("http://www02.eyny.com/" + canAnalysisAnimeList.first()["link"].toString());
    save(html);*/
}

void EynyWebControl::login(QString username, QString password)
{
    QJsonObject loginInfo = getLoginInfo();

    QStringList postData;
    postData << "formhash" << loginInfo["hash"].toString();
    postData << "referer" << "http://www02.eyny.com/index.php";
    postData << "loginfield" << "username";
    postData << "username" << username;
    postData << "password" << password;
    postData << "questionid" << "0";
    postData << "answer" << "";
    postData << "cookietime" << "2592000";

    QByteArray html = web->post(loginInfo["url"].toString(), WebAutomaticIndexer::toHtmlData(postData));
    //qDebug() << loginInfo["url"].toString();
    //qDebug() << loginInfo["hash"].toString();
    //save(html);
}

void EynyWebControl::login(QString username, QString password, std::function<void (void)> f)
{
    getLoginInfo([=](QJsonObject loginInfo){
        QStringList postData;
        postData << "formhash" << loginInfo["hash"].toString();
        postData << "referer" << "http://www02.eyny.com/index.php";
        postData << "loginfield" << "username";
        postData << "username" << username;
        postData << "password" << password;
        postData << "questionid" << "0";
        postData << "answer" << "";
        postData << "cookietime" << "2592000";

        web->post2(loginInfo["url"].toString(), WebAutomaticIndexer::toHtmlData(postData), QStringList(), [=](QByteArray html){
            f();
        });
    });

}

QJsonObject EynyWebControl::allAnimePageInfo(int needPage)
{
    QJsonObject allAnimeInfo;
    int articleNumberCount = 0;
    QString currentPageLink = "forum-22-1.html";

    int pageCount = 0;
    while(!currentPageLink.isEmpty()){
        qDebug() << ++pageCount << "http://www02.eyny.com/" + currentPageLink;
        emit updateInformation(pageCount);
        if(needPage != -1 && pageCount > needPage){
            break;
        }
        QByteArray animeHomePage = web->get("http://www02.eyny.com/" + currentPageLink);
        QString u_animeHomePage = codec->toUnicode(animeHomePage);
        //save(animeHomePage);

        QRegularExpression re("<tbody (.+?)</tbody>", QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatchIterator matchIt = re.globalMatch(u_animeHomePage);
        while(matchIt.hasNext()){
            QRegularExpressionMatch match = matchIt.next();
            QJsonObject animeInfo = animeBlock(urlFix(match.captured(1)));
            allAnimeInfo[QString::number(articleNumberCount++)] = animeInfo;
        }

        //if(articleNumberCount > 100) break;

        QString nextPageLink = capturedString(u_animeHomePage, QStringLiteral("</a><a href=\"(\\S+?)\" class=\"nxt\">下一頁</a>"));
        int badCount = 0;
        while(nextPageLink.isEmpty()){
            if(++badCount > 10){
                qDebug() << "can't get nextPageLink";
                break;
            }
            Asynchronous asynchronous;
            asynchronous.start(100);
            nextPageLink = capturedString(u_animeHomePage, QStringLiteral("</a><a href=\"(\\S+?)\" class=\"nxt\">下一頁</a>"));
        }
        currentPageLink = nextPageLink;
    }
    return allAnimeInfo;
}

QImage EynyWebControl::getImage(QString url)
{
    url.replace("/./", "/");

    QStringList header;
    header << "Referer" << "http://www02.eyny.com/forum-22-1.html";

    QByteArray html = web->get(url, header);

    return QImage::fromData(html);
}

void EynyWebControl::getImage(QString url, std::function<void (QImage)> f)
{
    url.replace("/./", "/");

    QStringList header;
    header << "Referer" << "http://www02.eyny.com/forum-22-1.html";

    web->get2(url, header, [=](QByteArray html){
        f(QImage::fromData(html));
    });
}

int EynyWebControl::getAnimeTotalPage()
{
    QByteArray animeHomePage = web->get("http://www02.eyny.com/forum-22-1.html");
    QString u_animeHomePage = codec->toUnicode(animeHomePage);

    return capturedString(u_animeHomePage, QStringLiteral("class=\"last\">... (.+?)</a>")).toInt();
}

template<typename F>
void globalMatchEx(QString str, QString pattern, F f){
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = re.globalMatch(str);
    while(it.hasNext()){
        QRegularExpressionMatch match = it.next();
        if(!match.captured(1).isEmpty())
            if(f(match.captured(1)) == true)
                break;
    }
}

QJsonObject EynyWebControl::aglaia123Analysis(QString url)
{
    qDebug() << url;

    QByteArray html = web->get(url);
    QString u_html = codec->toUnicode(html);
    save(html);

    QString imgUrl = urlFix(capturedString(u_html, "zoomfile=\"(.+?)\""));
    QString title = capturedString(u_html, QStringLiteral("【檔案名稱】：(.+?)<br />"));
    title.replace(QRegularExpression("<.+?>", QRegularExpression::DotMatchesEverythingOption), "");

    QJsonObject totalInfo;
    totalInfo["imgUrl"] = imgUrl;
    totalInfo["title"] = title;

    globalMatchEx(u_html, "<table(.+?)</table>", [&](QString match)->bool{
        bool stopLoop = false;

        if(match.contains(QStringLiteral("第１話"))){
            stopLoop = true;

            int count = 0;
            globalMatchEx(match, "<tr>(.+?)</tr>", [&](QString match)->bool{
                bool stopLoop = false;
                count++;

                if(count == 1){
                    return stopLoop;
                }

                QJsonObject downloadInfo;
                int fontCount = 0;
                globalMatchEx(match, "<td>(.+?)</td>", [&](QString match)->bool{
                    fontCount++;

                    QString text = capturedString(match, "<font color=\"#[0123456789abcdef]{6}\">(.+?)</font></font>");
                    if(text.isEmpty())
                        return true;

                    text.replace(QRegularExpression("<.+?>", QRegularExpression::DotMatchesEverythingOption), "");
                    text = urlFix(text);

                    switch(fontCount){
                    case 1: downloadInfo["language"] = text; break;
                    case 2: downloadInfo["subtitle group"] = text; break;
                    case 3: downloadInfo["format"] = text; break;
                    case 4: downloadInfo["quality"] = text; break;
                    default:{
                        int downloadCount = 0;
                        globalMatchEx(match, "<a href=\"(.+?)\"", [&](QString match)->bool{
                            ++downloadCount;
                            QJsonObject linkInfo;
                            if(!downloadInfo[QString::asprintf("link%2d", fontCount-4)].toObject().isEmpty()){
                                linkInfo = downloadInfo[QString::asprintf("link%2d", fontCount-4)].toObject();
                            }
                            linkInfo[QString::number(downloadCount)] = match;
                            downloadInfo[QString::asprintf("link%2d", fontCount-4)] = linkInfo;

                            //qDebug() << match;
                            return false;
                        });
                    }
                    }
                    //qDebug() << text;

                    return false;
                });

                if(!downloadInfo.isEmpty()){
                    totalInfo[downloadInfo["subtitle group"].toString()] = downloadInfo;
                }

                return stopLoop;
            });
        }

        return stopLoop;
    });

    return totalInfo;
}

QJsonObject EynyWebControl::aglaia123Analysis2(QString url){
    QByteArray html = web->get(url);
    QString u_html = codec->toUnicode(html);
    //save(html);

    QString imgUrl = urlFix(capturedString(u_html, "zoomfile=\"(.+?)\""));
    QString title = capturedString(u_html, QStringLiteral("【檔案名稱】：(.+?)<br />"));
    title.replace(QRegularExpression("<.+?>", QRegularExpression::DotMatchesEverythingOption), "");

    QJsonObject totalInfo;
    totalInfo["imgUrl"] = imgUrl;
    totalInfo["title"] = title;
    QJsonArray downloadUrlArray;

    QRegularExpression re("(http://www\\d*?.zippyshare.com.+?html)");
    QRegularExpressionMatchIterator it = re.globalMatch(u_html);
    while(it.hasNext()){
        QRegularExpressionMatch match = it.next();
        QString downloadUrl = match.captured(1);
        downloadUrlArray << downloadUrl;

        /*ZippyshareWebControl zip(downloadUrl);
        if(!zip.getEpisodeNumber().isEmpty()){
            QJsonObject subtitleGroupObject = totalInfo[zip.getSubtitleGroup()].toObject();
            subtitleGroupObject[zip.getEpisodeNumber()] = downloadUrl;
            totalInfo[zip.getSubtitleGroup()] = subtitleGroupObject;
        }*/
    }

    totalInfo["downloadUrl"] = downloadUrlArray;

    return totalInfo;
}

// catch all zippyshare url
void EynyWebControl::aglaia123Analysis2(QString url, std::function<void (QJsonObject)> f)
{
    web->get2(url, QStringList(), [=](QByteArray html){
        QString u_html = codec->toUnicode(html);

        QString imgUrl = urlFix(capturedString(u_html, "zoomfile=\"(.+?)\""));
        QString title = capturedString(u_html, QStringLiteral("【檔案名稱】：(.+?)<br />"));
        title.replace(QRegularExpression("<.+?>", QRegularExpression::DotMatchesEverythingOption), "");
        title = urlFix(title);

        QJsonObject totalInfo;
        totalInfo["imgUrl"] = imgUrl;
        totalInfo["title"] = title.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");;
        QJsonArray downloadUrlArray;

        QRegularExpression re("(http://www\\d*?.zippyshare.com.+?html)");
        QRegularExpressionMatchIterator it = re.globalMatch(u_html);
        while(it.hasNext()){
            QRegularExpressionMatch match = it.next();
            QString downloadUrl = match.captured(1);
            downloadUrlArray << downloadUrl;
        }

        totalInfo["downloadUrl"] = downloadUrlArray;

        f(totalInfo);
    });
}

void EynyWebControl::aglaia123Analysis3(QString url, std::function<void (QString /*title*/, QStringList /*downloadUrls*/)> f)
{
    web->get2(url, QStringList(), [=](QByteArray html){
        //QTime t = QTime::currentTime();
        QString u_html = codec->toUnicode(html);

        QString imgUrl = urlFix(capturedString(u_html, "zoomfile=\"(.+?)\""));
        QString title = capturedString(u_html, QStringLiteral("【檔案名稱】：(.+?)<br />"));
        title.replace(QRegularExpression("<.+?>", QRegularExpression::DotMatchesEverythingOption), "");
        title = urlFix(title);

        QStringList downloadUrls;

        QRegularExpression re("(http://www\\d*?.zippyshare.com.+?html)");
        QRegularExpressionMatchIterator it = re.globalMatch(u_html);
        while(it.hasNext()){
            QRegularExpressionMatch match = it.next();
            QString downloadUrl = match.captured(1);
            downloadUrls << downloadUrl;
        }

        //qDebug() << "a" << t.secsTo(QTime::currentTime());
        f(title, downloadUrls);
    });
}

void EynyWebControl::home()
{
    QByteArray html = web->get("http://www02.eyny.com/index.php");
}

QJsonObject EynyWebControl::getLoginInfo()
{
    QJsonObject loginInfo;

    QByteArray html = web->get("http://www02.eyny.com/member.php?mod=logging&action=login&infloat=yes&handlekey=login&inajax=1&ajaxtarget=fwin_content_login");
    QString u_html = codec->toUnicode(html);

    //save(html);

    loginInfo["url"] = urlFix("http://www02.eyny.com/" + capturedString(u_html, "action=\"(.+?)\"") + "&inajax=1");
    loginInfo["hash"] = capturedString(u_html, "name=\"formhash\" value=\"(.+?)\"");

    return loginInfo;
}

void EynyWebControl::getLoginInfo(std::function<void (QJsonObject)> f)
{
    QJsonObject loginInfo;

    web->get2("http://www02.eyny.com/member.php?mod=logging&action=login&infloat=yes&handlekey=login&inajax=1&ajaxtarget=fwin_content_login", QStringList(), [=](QByteArray html){
        QString u_html = codec->toUnicode(html);

        QJsonObject loginInfo;
        loginInfo["url"] = urlFix("http://www02.eyny.com/" + capturedString(u_html, "action=\"(.+?)\"") + "&inajax=1");
        loginInfo["hash"] = capturedString(u_html, "name=\"formhash\" value=\"(.+?)\"");
        f(loginInfo);
    });
}

QJsonObject EynyWebControl::animeBlock(QString block)
{
    QJsonObject animeInfo;

    QString link = capturedString(block, "<td class=\"icn\">.*?<a href=\"(thread.+?)\"");
    QString title = capturedString(block, "class=\"xst\" >(.+?)</a>");
    //QString title2 = capturedString(block, "<th class=.+?<a href=\"thread.+?>(.+?)</a>");
    //QString title3 = capturedString(block, "<td class=\"p_pre_td\">.+?alt='(.+?)'");
    QString imageUrl = capturedString(block, "<td class=\"p_pre_td\">.+?<img src='(.+?)'");
    QString author = capturedString(block, "<td class=\"by\">.*?<cite>.*?>(.+?)</a>.*?</cite>");
    QString leaveMessageNumber = capturedString(block, "<td class=\"num\">.*?>(.+?)</a>");
    QString browseNumber = capturedString(block, "<td class=\"num\">.*?<em>(.+?)</em>");
    QString releaseDate = capturedString(block, "<td class=\"by\">.*?<span>(.+?)</span>");
    QString tag = capturedString(block, "<th class=\".*?<em>\\[.*?>(.+?)</a>.*?</em>");

    animeInfo["link"] = link;
    animeInfo["title"] = title;
    animeInfo["imageUrl"] = imageUrl;
    animeInfo["author"] = author;
    animeInfo["leaveMessageNumber"] = leaveMessageNumber;
    animeInfo["browseNumber"] = browseNumber;
    animeInfo["releaseDate"] = releaseDate;
    animeInfo["tag"] = tag;

    return animeInfo;
}
