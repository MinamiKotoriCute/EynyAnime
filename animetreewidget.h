#ifndef ANIMETREEWIDGET_H
#define ANIMETREEWIDGET_H

//http://blog.csdn.net/ma12shuai/article/details/8662572

#include <functional>

#include <QWidget>
#include <QMap>
#include <QTreeWidget>

#include "zippysharewebcontrol.h"
#include "animeinfo.h"

class QLabel;
class QTreeWidgetItem;

class AnimeTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit AnimeTreeWidget(QWidget *parent = 0);

    void addAnime(QString eynyUrl);

    QTreeWidgetItem* addItem(QString text);
    QTreeWidgetItem* addItem(QStringList text, QTreeWidgetItem *parent);

signals:
    void favoriteChanged(bool isFavorite);

public slots:

private:
    bool changeFromUser;
    bool changeToParent;

private slots:
    void treeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void changeParents(QTreeWidgetItem *item, Qt::CheckState state);
    void changeChildren(QTreeWidgetItem *item, Qt::CheckState state);
};

#endif // ANIMETREEWIDGET_H
