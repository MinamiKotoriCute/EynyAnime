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
#include "myqsettings.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), eyny(new EynyWebControl(this))
{
    animeListWidget = new AnimeListWidget;
    setCentralWidget(animeListWidget);

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
    });

    readSettings();
    loadAnimeData();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *)
{
    writeSettings();
}

void MainWindow::readSettings()
{
    MyQSettings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    MyQSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::loadAnimeData()
{
    QJsonObject allAnimeInfo = loadJson();
    QList<QJsonObject> canAnalysisAnimeList;

    for(QString key : allAnimeInfo.keys()){
        QJsonObject animeInfo = allAnimeInfo.value(key).toObject();
        if(animeInfo["author"].toString() == "aglaia123"){
            canAnalysisAnimeList.push_back(animeInfo);
        }
    }
    animeListWidget->setAnimeList(canAnalysisAnimeList);
}
