#include "animeinfo.h"

#include <QRegularExpression>

AnimeInfo::AnimeInfo()
{

}

void AnimeInfo::analysis(QString zipFullName){
    this->zipFullName = zipFullName;
    QRegularExpression re("\\]?\\[?(.+?)[\\]\\[]", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = re.globalMatch(zipFullName);
    while(it.hasNext()){
        QRegularExpressionMatch match = it.next();
        QString cap = match.captured(1);
        //qDebug() << cap;

        bool ok;
        cap.toInt(&ok);
        if(ok){
            zipEpisodeNumber = cap;
        }
        else if(zipEpisodeNumber.isEmpty() && cap.contains(QRegularExpression("\\d{2}", QRegularExpression::DotMatchesEverythingOption))){
            zipEpisodeNumber = cap;
        }
        else if(zipQuality.isEmpty() && cap.contains(QRegularExpression("\\d.\\d", QRegularExpression::DotMatchesEverythingOption))){
            zipQuality = cap;
        }
        else if(zipSubtitleGroup.isEmpty()){
            zipSubtitleGroup = cap;
        }
        else if(zipFileName.isEmpty()){
            zipFileName = cap;
        }
    }
}
