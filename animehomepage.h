#ifndef ANIMEHOMEPAGE_H
#define ANIMEHOMEPAGE_H

#include <QWidget>

#include "animeinfo.h"

class AnimeHomePage : public QWidget
{
    Q_OBJECT
public:
    explicit AnimeHomePage(QWidget *parent = 0);

signals:

public slots:

private:
    QVector<AnimeInfo> infos;
};

#endif // ANIMEHOMEPAGE_H
