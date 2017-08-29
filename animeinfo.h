#ifndef ANIMEINFO_H
#define ANIMEINFO_H

#include <QString>
#include <QDebug>

class AnimeInfo
{
public:
    AnimeInfo();

    void analysis(QString zipFullName);

    /*inline bool isComplete(){
        if(!episodeNumber.isEmpty() && !quality.isEmpty())
            return true;
        return false;
    }

    bool isEmpty(){
        if(fileName.isEmpty() || episodeNumber.isEmpty())
            return true;
        return false;
    }*/

    friend QDebug operator<<(QDebug d, const AnimeInfo &info){
        d.nospace() << "{eynyUrl:" << info.eynyUrl
                    << ", eynyAnimeName:" << info.eynyAnimeName
                    << ", zipUrl:" << info.zipUrl
                    << ", zipDownloadUrl:" << info.zipDownloadUrl
                    << ", zipFullName:" << info.zipFullName
                    << ", zipFileName:" << info.zipFileName
                    << ", zipEpisodeNumber:" << info.zipEpisodeNumber
                    << ", zipSubtitleGroup:" << info.zipSubtitleGroup
                    << ", zipQuality:" << info.zipQuality << "}";
        return d;
    }

    QString eynyUrl;
    QString eynyAnimeName;
    QString zipUrl;
    QString zipDownloadUrl;
    QString zipFullName;
    QString zipFileName;
    QString zipEpisodeNumber;
    QString zipSubtitleGroup;
    QString zipQuality;
};

#endif // ANIMEINFO_H
