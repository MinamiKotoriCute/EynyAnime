#include "downloadwidget.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QProcess>
#include <QDir>
#include <QDebug>

#include "eynywebcontrol.h"

DownloadWidget &DownloadWidget::Instance()
{
    static DownloadWidget w;
    return w;
}

DownloadWidget::DownloadWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Favorite");

    baseDir = "Anime/";
    if(!QDir(baseDir).exists()){
        QDir().mkdir(baseDir);
        qDebug() << "make dir anime";
    }

    treeWidget = new TreeWidget;
    treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    treeWidget->setSortingEnabled(true);
    treeWidget->sortByColumn(0, Qt::AscendingOrder);
    treeWidget->setHeaderLabels({"name / subtitleGroup / episodeNumber", "fileNumber / maxEpisodeNumber"});
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget->setSelectionMode(QTreeWidget::NoSelection);
    treeWidget->setHasCheckBox(true);

    QVBoxLayout *vb = new QVBoxLayout;
    vb->addWidget(treeWidget);
    setLayout(vb);

    connect(treeWidget, &QTreeWidget::customContextMenuRequested, [=](const QPoint &p){
        QTreeWidgetItem *item = treeWidget->itemAt(p);
        if(item){
            QTreeWidgetItem *hasInfoItem = item;
            while(hasInfoItem->childCount() != 0)
                hasInfoItem = hasInfoItem->child(0);
            AnimeInfo info = mapToAnimeInfo[hasInfoItem];

            if(item->parent() == 0){
                QMenu *topLevelItemMenu = new QMenu;
                connect(topLevelItemMenu, &QMenu::aboutToHide, topLevelItemMenu, &QMenu::deleteLater);
                topLevelItemMenu->addAction("go to web", [=]{
                    QProcess::startDetached("cmd /c \"start " + info.eynyUrl + "\"");
                });
                topLevelItemMenu->addAction("show in explore", [=]{
                    QString animeName = info.eynyAnimeName;
                    QString path = QDir::currentPath() + "/" + baseDir + animeName.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");
                    path = path.replace("/", "\\");
                    //qDebug() << path;
                    if(QDir(path).exists()){
                        QProcess::startDetached("explorer \"" + path + "\"");
                    }
                    else{
                        qDebug() << "path not exist:" << path;
                    }
                });
                topLevelItemMenu->addAction("delete", [=]{
                    delete treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(item));
                });

                // Note: We must map the point to global from the viewport to
                // account for the header.
                topLevelItemMenu->popup(treeWidget->viewport()->mapToGlobal(p));
            }
            else if(item->childCount() == 0){
                QMenu *topLevelItemMenu = new QMenu;
                connect(topLevelItemMenu, &QMenu::aboutToHide, topLevelItemMenu, &QMenu::deleteLater);
                topLevelItemMenu->addAction("open file", [=]{
                    QString animeName = info.eynyAnimeName;
                    QString path = QDir::currentPath() + "/" + baseDir + animeName.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");
                    path = path.replace("/", "\\");
                    QFileInfo fileInfo(path, info.zipFullName);
                    qDebug() << fileInfo.filePath();
                    if(fileInfo.exists()){
                        QString filePath = fileInfo.filePath().replace("&", "^&");
                        QProcess::startDetached("cmd", QStringList() << "/c" << "start" << "" << filePath);
                    }
                });

                topLevelItemMenu->popup(treeWidget->viewport()->mapToGlobal(p));
            }
        }
    });

    connect(treeWidget, &TreeWidget::itemCheckStateChanged, [=](QTreeWidgetItem *item){
        if(item->childCount() == 0){
            AnimeInfo info = mapToAnimeInfo[item];
            QString path = info.eynyAnimeName.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");
            QFileInfo fileInfo(baseDir + path, info.zipFullName);
            if(fileInfo.exists()){
                //qDebug() << "file exist, path:" << path << " info:" << info;
                return;
            }
            qDebug() << info;

            if(item->checkState(0) == Qt::Checked){
                item->setText(1, "waiting...");

                downloader.addDownloadList(info.zipUrl, fileInfo, [=](qint64 bytesReceived, qint64 bytesTotal){
                    double percent = (double)bytesReceived / (double)bytesTotal * 100;
                    QString sentUnit = "b", totalUnit = "b";
                    if(bytesReceived / 1024 > 0){
                        bytesReceived /= 1024;
                        sentUnit = "kb";
                    }
                    if(bytesReceived / 1024 > 0){
                        bytesReceived /= 1024;
                        sentUnit = "mb";
                    }
                    if(bytesTotal / 1024 > 0){
                        bytesTotal /= 1024;
                        totalUnit = "kb";
                    }
                    if(bytesTotal / 1024 > 0){
                        bytesTotal /= 1024;
                        totalUnit = "mb";
                    }
                    QString text = QString::asprintf("%3.2lf%%    ", percent);
                    text += QString::asprintf("%4ld", bytesReceived) + sentUnit;
                    text += QString::asprintf(" / %4ld", bytesTotal) + totalUnit;
                    item->setText(1, text);
                    //qDebug() << (double)bytesReceived / (double)bytesTotal * 100 << bytesReceived << bytesTotal;
                });

                /*ZippyshareWebControl zip(info.zipUrl);

                connect(&zip, &ZippyshareWebControl::downloadProgress, [=](qint64 bytesReceived, qint64 bytesTotal){
                    double percent = (double)bytesReceived / (double)bytesTotal * 100;
                    QString sentUnit = "b", totalUnit = "b";
                    if(bytesReceived / 1024 > 0){
                        bytesReceived /= 1024;
                        sentUnit = "kb";
                    }
                    if(bytesReceived / 1024 > 0){
                        bytesReceived /= 1024;
                        sentUnit = "mb";
                    }
                    if(bytesTotal / 1024 > 0){
                        bytesTotal /= 1024;
                        totalUnit = "kb";
                    }
                    if(bytesTotal / 1024 > 0){
                        bytesTotal /= 1024;
                        totalUnit = "mb";
                    }
                    QString text = QString::asprintf("%3.2lf%%    ", percent);
                    text += QString::asprintf("%4ld", bytesReceived) + sentUnit;
                    text += QString::asprintf(" / %4ld", bytesTotal) + totalUnit;
                    item->setText(1, text);
                    //qDebug() << (double)bytesReceived / (double)bytesTotal * 100 << bytesReceived << bytesTotal;
                });

                zip.download(fileInfo, 0, 100000);*/
            }
            else if(item->checkState(0) == Qt::Unchecked){
                item->setText(1, info.zipFullName);
            }
        }
    });

    /*animeTreeWidget->setAddAnimeFunction([this](AnimeInfo animeInfo){
        if(animeInfo.zipFullName.isEmpty())
            return;
        //qDebug() << animeInfo;

        QTreeWidgetItem *animeItem = animeTreeWidget->addItem(animeInfo.eynyAnimeName);
        QTreeWidgetItem *subtitleGroupItem = animeTreeWidget->addItem({animeInfo.zipSubtitleGroup}, animeItem);
        QTreeWidgetItem *episodeNumber = animeTreeWidget->addItem({animeInfo.zipEpisodeNumber, animeInfo.zipFullName}, subtitleGroupItem);

        subtitleGroupItem->setText(0, animeInfo.zipSubtitleGroup + QString::asprintf(" (%d)", subtitleGroupItem->childCount()));

        animeTreeWidget->getMapToAnimeInfo()[animeItem] = animeInfo;
    });*/

    readSettings();
}

DownloadWidget::~DownloadWidget()
{
}

void DownloadWidget::addAnime(QString eynyUrl)
{
    EynyWebControl *eynyLogin = new EynyWebControl;
    eynyLogin->login(username, password, [=]{
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
                    addAnime(animeInfo);
                    //qDebug() << "AnimeTreeWidget::addAnime" << animeInfo;
                    //zip->deleteLater();
                }, zipUrl);
            }

            eynyLogin->deleteLater();
        });
    });
}

void DownloadWidget::addAnime(AnimeInfo animeInfo)
{
    if(animeInfo.zipFullName.isEmpty())
        return;

    QTreeWidgetItem *animeItem = treeWidget->addItem(animeInfo.eynyAnimeName);
    QTreeWidgetItem *subtitleGroupItem = treeWidget->addItem({animeInfo.zipSubtitleGroup}, animeItem);
    QTreeWidgetItem *episodeNumber = treeWidget->addItem({animeInfo.zipEpisodeNumber, animeInfo.zipFullName}, subtitleGroupItem);

    subtitleGroupItem->setText(0, animeInfo.zipSubtitleGroup + QString::asprintf(" (%d)", subtitleGroupItem->childCount()));

    mapToAnimeInfo[episodeNumber] = animeInfo;

    QString path = baseDir + animeInfo.eynyAnimeName.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");
    QFileInfo fileInfo(path, animeInfo.zipFullName);
    if(fileInfo.dir().exists()){
        if(fileInfo.exists())
            episodeNumber->setCheckState(0, Qt::Checked);

        int total = 0;
        for(int i=0;i<animeItem->childCount();i++){
            QTreeWidgetItem *childItem = animeItem->child(i);
            for(int j=0;j<childItem->childCount();j++){
                bool ok;
                int n = childItem->child(j)->text(0).toInt(&ok);
                if(ok && total < n){
                    total = n;
                }
            }
        }

        int fileNumber = 0;
        QDir dir = fileInfo.dir();
        dir.setFilter(QDir::Files);
        const QFileInfoList fileList = dir.entryInfoList();
        for (int i = 0; i < fileList.size(); i++) {
            QFileInfo fileInfo = fileList.at(i);
            if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." ); // nothing
            else {
                ++fileNumber;
            }
        }

        /*for(int i=0;i<animeItem->childCount();i++){
            max += animeItem->child(i)->text(1).toInt();
        }*/
        animeItem->setText(1, QString::asprintf("%d / %d", fileNumber, total));
    }

    int num = 0;
    for(int i=0;i<subtitleGroupItem->childCount();i++){
        if(subtitleGroupItem->child(i)->checkState(0) == Qt::Checked)
            ++num;
    }
    if(num != 0)
        subtitleGroupItem->setText(1, QString::number(num));

    writeSettings();
}

void DownloadWidget::closeEvent(QCloseEvent *)
{
    writeSettings();
}

void DownloadWidget::readSettings()
{
    //QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ASD", "EynyAnime");
    QSettings settings("Information.ini",QSettings::IniFormat);

    settings.beginGroup("DownloadWidget");
    username = settings.value("username").toString();
    password = settings.value("password").toString();

    int size = settings.beginReadArray("urls");
    for(int i=0;i<size;i++){
        settings.setArrayIndex(i);
        QString eynyUrl = settings.value("url").toString();

        addAnime(eynyUrl);
    }
    settings.endArray();

    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(600, 200)).toPoint());

    settings.endGroup();
}

void DownloadWidget::writeSettings()
{
    //QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ASD", "EynyAnime");
    QSettings settings("Information.ini",QSettings::IniFormat);
    settings.clear();
    settings.beginGroup("DownloadWidget");
    settings.setValue("username", username);
    settings.setValue("password", password);
    settings.beginWriteArray("urls");
    for(int i=0;i<treeWidget->topLevelItemCount();i++){
        settings.setArrayIndex(i);
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        while(item->childCount() != 0)
            item = item->child(0);
        QString url = mapToAnimeInfo[item].eynyUrl;
        //qDebug() << "save" << url << urlMap[item] << urlMap.value(item) << item;
        settings.setValue("url", url);
    }
    settings.endArray();

    settings.setValue("size", size());
    settings.setValue("pos", pos());

    settings.endGroup();
}
