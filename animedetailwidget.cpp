#include "animedetailwidget.h"

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

AnimeDetailWidget::AnimeDetailWidget(QWidget *parent) : QWidget(parent), changeFromUser(true)
{
    titleLabel = new QLabel("Loaging...");
    imageLabel = new QLabel("Loaging...");

    treeWidget = new QTreeWidget;
    treeWidget->setSortingEnabled(true);
    treeWidget->sortByColumn(0, Qt::AscendingOrder);
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeWidget->setHeaderLabels({"Key", "Value"});
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget->setSelectionMode(QTreeWidget::NoSelection);

    QVBoxLayout *vb = new QVBoxLayout;
    vb->addWidget(titleLabel);
    vb->addWidget(imageLabel);

    QPushButton *button = new QPushButton("go to web");
    QPushButton *button2 = new QPushButton("add to favorite");
    QHBoxLayout *hb2 = new QHBoxLayout;
    hb2->addWidget(button);
    hb2->addWidget(button2);

    QVBoxLayout *vb2 = new QVBoxLayout;
    vb2->addLayout(hb2);
    vb2->addWidget(treeWidget);

    QHBoxLayout *hb = new QHBoxLayout;
    hb->addLayout(vb2);
    hb->addLayout(vb);
    setLayout(hb);

    connect(treeWidget, &QTreeWidget::itemChanged, this, &AnimeDetailWidget::treeWidget_itemChanged);

    connect(button, &QPushButton::clicked, [=]{
        QProcess::startDetached("cmd /c \"start " + pageUrl + "\"");
    });

    connect(button2, &QPushButton::clicked, [=]{
        emit favoriteChanged(true);
    });
}

void AnimeDetailWidget::setInfo(QString infoUrl)
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
}

QTreeWidgetItem* AnimeDetailWidget::addItem(QString text)
{
    QTreeWidgetItem *item = nullptr;
    for(int i=0;i<treeWidget->topLevelItemCount();i++){
        if(treeWidget->topLevelItem(i)->text(0).contains(text)){
            item = treeWidget->topLevelItem(i);
            break;
        }
    }

    if(item == nullptr){
        item = new QTreeWidgetItem(QStringList(text));
        //item->setCheckState(0, Qt::Unchecked);
        treeWidget->addTopLevelItem(item);

    }

    item->setText(0, text + QString::asprintf(" (%d)", item->childCount() + 1));

    return item;
}

QTreeWidgetItem *AnimeDetailWidget::addItem(QStringList text, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = nullptr;
    for(int i=0;i<parent->childCount();i++){
        if(parent->child(i)->text(0) == text.first()){
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

void AnimeDetailWidget::treeWidget_itemChanged(QTreeWidgetItem *item, int column)
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

void AnimeDetailWidget::changeParents(QTreeWidgetItem *item, Qt::CheckState state)
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

void AnimeDetailWidget::changeChildren(QTreeWidgetItem *item, Qt::CheckState state)
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

void AnimeDetailWidget::add(AnimeInfo info)
{
    QString subtitleGroup = info.zipSubtitleGroup;
    if(subtitleGroup.isEmpty())
        return;
    QTreeWidgetItem *subtitleGroupItem = addItem(subtitleGroup);

    QTreeWidgetItem *episodeNumberItem = addItem({info.zipEpisodeNumber, info.zipFullName}, subtitleGroupItem);
    //downloadLink[episodeNumberItem] = info.url;
    downloadInfos[episodeNumberItem] = info;

    subtitleGroupItem->setText(0, subtitleGroup + QString::asprintf(" (%d)", subtitleGroupItem->childCount()));
}
