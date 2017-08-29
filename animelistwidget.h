#ifndef ANIMELISTWIDGET_H
#define ANIMELISTWIDGET_H

#include <QWidget>

class QListWidget;
class WebAutomaticIndexer;
class EynyWebControl;

class AnimeListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnimeListWidget(QWidget *parent = 0);

    void setAnimeList(QList<QJsonObject> animeList);

signals:
    void listWidget_itemDoubleClicked(QString downloadUrl);

public slots:

private:
    EynyWebControl *eyny;
    QList<QJsonObject> animeList;
    WebAutomaticIndexer *web;
    QListWidget *listWidget;
};

#endif // ANIMELISTWIDGET_H
