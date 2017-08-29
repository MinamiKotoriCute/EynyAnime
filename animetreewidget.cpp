#include "animetreewidget.h"

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QProcess>
#include <QHeaderView>
#include <QDebug>

#include "eynywebcontrol.h"
#include "zippysharewebcontrol.h"

AnimeTreeWidget::AnimeTreeWidget(QWidget *parent) : QTreeWidget(parent), changeFromUser(true)
{
    connect(this, &QTreeWidget::itemChanged, this, &AnimeTreeWidget::treeWidget_itemChanged);
/*
    addAnimeFunction = [=](AnimeInfo animeInfo){
        if(animeInfo.zipEpisodeNumber.isEmpty() || animeInfo.eynyAnimeName.isEmpty())
            return;

        QTreeWidgetItem *animeNameItem = addItem({animeInfo.eynyAnimeName});
        mapToAnimeInfo[animeNameItem] = animeInfo;

        QTreeWidgetItem *subtitleGroupItem = addItem({animeInfo.zipSubtitleGroup}, animeNameItem);
        addItem({animeInfo.zipEpisodeNumber}, subtitleGroupItem);

        subtitleGroupItem->setText(0, animeInfo.zipSubtitleGroup + QString::asprintf(" (%d)", subtitleGroupItem->childCount()));
    };*/
}

void AnimeTreeWidget::addAnime(QString eynyUrl)
{
    EynyWebControl *eynyLogin = new EynyWebControl;
    eynyLogin->login("joeyten2000", "sophie10");
    eynyLogin->aglaia123Analysis2(eynyUrl, [=](QJsonObject infoObject){
        QJsonArray array = infoObject["downloadUrl"].toArray();
        for(const QJsonValue &value : array){
            const QString zipUrl = value.toString();
            ZippyshareWebControl::getTitle2([=](QString title){
                AnimeInfo animeInfo;
                animeInfo.analysis(title);
                animeInfo.eynyAnimeName = infoObject["title"].toString();
                animeInfo.zipUrl = zipUrl;
                animeInfo.eynyUrl = eynyUrl;
                //addAnimeFunction(animeInfo);
                //qDebug() << "AnimeTreeWidget::addAnime" << animeInfo;
                //zip->deleteLater();
            }, zipUrl);
        }

        eynyLogin->deleteLater();
    });
}

/*
void AnimeTreeWidget::setInfo(QString infoUrl)
{
    pageUrl = infoUrl;

    QJsonObject infoObject;
    {
        EynyWebControl eyny;
        eyny.login("joeyten2000", "sophie10");
        infoObject = eyny.aglaia123Analysis2(pageUrl);
    }

    treeWidget->clear();
    //downloadLink.clear();
    downloadInfos.clear();

    EynyWebControl eyny;
    titleLabel->setText(infoObject["title"].toString());
    eyny.getImage2(infoObject["imgUrl"].toString(), [=](QImage img){
        QPixmap pixmap = QPixmap::fromImage(img);
        if(pixmap.height() > 500)
            pixmap = pixmap.scaled(QSize(pixmap.width(), 500), Qt::KeepAspectRatio);
        imageLabel->setPixmap(pixmap);
    });

    QJsonArray array = infoObject["downloadUrl"].toArray();
    for(const QJsonValue &value : array){
        const QString url = value.toString();

        //ZippyshareWebControl *zip = new ZippyshareWebControl(url);
        ZippyshareWebControl::getTitle2([=](QString title){
            AnimeInfo animeInfo;
            animeInfo.analysis(title);
            animeInfo.eynyAnimeName = infoObject["title"].toString();
            animeInfo.zipUrl = url;
            animeInfo.eynyUrl = infoUrl;
            add(animeInfo);
            //qDebug() << animeInfo;
            //zip->deleteLater();
        }, url);
    }
}*/

QTreeWidgetItem* AnimeTreeWidget::addItem(QString text)
{
    QTreeWidgetItem *item = nullptr;
    for(int i=0;i<topLevelItemCount();i++){
        if(topLevelItem(i)->text(0).contains(text)){
            item = topLevelItem(i);
            break;
        }
    }

    if(item == nullptr){
        item = new QTreeWidgetItem(QStringList(text));
        //item->setCheckState(0, Qt::Unchecked);
        addTopLevelItem(item);
    }

    return item;
}

QTreeWidgetItem *AnimeTreeWidget::addItem(QStringList text, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = nullptr;
    for(int i=0;i<parent->childCount();i++){
        if(parent->child(i)->text(0).contains(text.first())){
            bool isEqual = true;
            for(int j=1;j<text.size() && j<parent->child(i)->columnCount();j++){
                if(text.at(j) != parent->child(i)->text(j)){
                    isEqual = false;
                    break;
                }
            }
            if(!isEqual)
                continue;

            item = parent->child(i);
            break;
        }
    }

    if(item == nullptr){
        item = new QTreeWidgetItem(text);
        //item->setCheckState(0, Qt::Unchecked);
        parent->addChild(item);
    }

    return item;
}

void AnimeTreeWidget::treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;
    if (changeFromUser)
    {
        changeFromUser = false;
        changeToParent = true;

        changeParents(item, item->checkState(0));
        changeChildren(item, item->checkState(0));
        changeFromUser = true;

        if(item->childCount() == 0){
            //emit downloadStateChanged(item->checkState(0)==Qt::Checked?true:false, downloadLink[item], item);
            //emit downloadStateChanged(item->checkState(0)==Qt::Checked?true:false, downloadInfos[item], item);
        }
    }
    else
    {
        if (changeToParent) changeParents(item, item->checkState(0));
        else changeChildren(item, item->checkState(0));
    }
}

void AnimeTreeWidget::changeParents(QTreeWidgetItem *item, Qt::CheckState state)
{
    QTreeWidgetItem *iParent = item->parent();
    if (iParent == 0)
    {
        changeToParent = false;
        return;
    }
    else
    {
        int i,k=iParent->childCount();
        for (i=0;i<k;i++)
        {
            if (iParent->child(i)->checkState(0) != state)
                break;
        }

        if (i==k) iParent->setCheckState(0, state);
        else iParent->setCheckState(0, Qt::PartiallyChecked);
    }
}

void AnimeTreeWidget::changeChildren(QTreeWidgetItem *item, Qt::CheckState state)
{
    int i,k=item->childCount();
    for (i=0;i<k;i++)
    {
        if(item->child(i)->checkState(0) != state){
            item->child(i)->setCheckState(0, state);
            if(item->child(i)->childCount() == 0){
                //emit downloadStateChanged(state==Qt::Checked?true:false, downloadLink[item->child(i)], item->child(i));
                //emit downloadStateChanged(state==Qt::Checked?true:false, downloadInfos[item->child(i)], item->child(i));
            }
        }
    }
}
