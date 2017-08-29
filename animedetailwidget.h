#ifndef ANIMEDETAILWIDGET_H
#define ANIMEDETAILWIDGET_H

#include <QWidget>
#include <QMap>

#include "zippysharewebcontrol.h"
#include "animeinfo.h"

class QTreeWidget;
class QLabel;
class QTreeWidgetItem;

class AnimeDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnimeDetailWidget(QWidget *parent = 0);

    void setInfo(QString infoUrl);
    void setAnime(QJsonObject animeDownloadInfo);

signals:
    //void downloadStateChanged(bool isAdd, QString url, QTreeWidgetItem *item);
    //void downloadStateChanged(bool isAdd, AnimeInfo info, QTreeWidgetItem *item);
    void favoriteChanged(bool isFavorite);

public slots:

private:
    void add(AnimeInfo info);
    QTreeWidgetItem* addItem(QString text);
    QTreeWidgetItem* addItem(QStringList text, QTreeWidgetItem *parent);

    bool changeFromUser;
    bool changeToParent;
    QLabel *titleLabel;
    QLabel *imageLabel;
    QTreeWidget *treeWidget;
    //QMap<void *, QString> downloadLink;
    QMap<void *, AnimeInfo> downloadInfos;
    QString pageUrl;

private slots:
    void treeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void changeParents(QTreeWidgetItem *item, Qt::CheckState state);
    void changeChildren(QTreeWidgetItem *item, Qt::CheckState state);
};

#endif // ANIMEDETAILWIDGET_H
