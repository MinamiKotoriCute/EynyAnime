#include "mainwindow.h"

#include <QJsonObject>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QDebug>

#include "animedetailwidget.h"
#include "animelistwidget.h"
#include "commonfunction.h"
#include "zippysharewebcontrol.h"
#include "downloadwidget.h"
#include "animeinfo.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), eyny(new EynyWebControl(this))
{

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


    AnimeListWidget *animeListWidget = new AnimeListWidget;
    setCentralWidget(animeListWidget);

    animeListWidget->setAnimeList(canAnalysisAnimeList);


    connect(animeListWidget, &AnimeListWidget::listWidget_itemDoubleClicked, [=](QString downloadUrl){
        AnimeDetailWidget *animeDetailWidget = new AnimeDetailWidget;
        animeDetailWidget->show();

        //QJsonObject animeDownloadInfo = eyny->aglaia123Analysis2(downloadUrl);
        animeDetailWidget->setInfo(downloadUrl);

        connect(animeDetailWidget, &AnimeDetailWidget::favoriteChanged, [=](bool isFavorite){
            if(isFavorite){
                DownloadWidget &downloadWidget = DownloadWidget::Instance();
                downloadWidget.addAnime(downloadUrl);
                downloadWidget.show();
            }
        });
/*
        connect(treeWidget, &AnimeTreeWidget::downloadStateChanged, [=](bool isAdd, AnimeInfo info, QTreeWidgetItem *item){

            if(isAdd){
                ZippyshareWebControl zip(info.zipUrl);

                DownloadWidget &downloadWidget = DownloadWidget::Instance();
                downloadWidget.add(info);
                downloadWidget.show();

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

                QString path = info.eynyAnimeName.replace(QRegularExpression("[/\\?\\*<\":>]"), "_");
                QFileInfo fileInfo(path, info.zipFullName);
                zip.download(fileInfo);
            }
        });*/
    });

    readSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    writeSettings();
}

void MainWindow::readSettings()
{
    //QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MainWindow", "info");
    QSettings settings("Information.ini",QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    //QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MainWindow", "info");
    QSettings settings("Information.ini",QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}
