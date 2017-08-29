#include "animelistwidget.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QLineEdit>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QSlider>
#include <QDebug>
#include <QPushButton>
#include <QJsonObject>
#include <QProgressBar>
#include <QCheckBox>
#include <QAction>

#include "asynchronous.h"
#include "eynywebcontrol.h"
#include "animetreewidget.h"
#include "commonfunction.h"

AnimeListWidget::AnimeListWidget(QWidget *parent) : QWidget(parent), web(new WebAutomaticIndexer), eyny(new EynyWebControl)
{
    QLabel *label4 = new QLabel("progress rate");
    QProgressBar *processBar = new QProgressBar;
    QPushButton *button = new QPushButton("update");
    QHBoxLayout *hb4 = new QHBoxLayout;
    hb4->addWidget(label4);
    hb4->addWidget(processBar);
    hb4->addWidget(button);

    QCheckBox *checkBox = new QCheckBox("Show Icon");

    QLabel *label2 = new QLabel("search");
    QLineEdit *edit = new QLineEdit;
    QHBoxLayout *hb2 = new QHBoxLayout;
    hb2->addWidget(label2);
    hb2->addWidget(edit);

    QLabel *label3 = new QLabel("image size");
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(18, 99);
    slider->setValue(18);
    QHBoxLayout *hb3 = new QHBoxLayout;
    hb3->addWidget(label3);
    hb3->addWidget(slider);

    listWidget = new QListWidget;
    listWidget->setMouseTracking(true);

    QVBoxLayout *vb = new QVBoxLayout;
    vb->addLayout(hb4);
    vb->addWidget(checkBox);
    vb->addLayout(hb2);
    vb->addLayout(hb3);
    vb->addWidget(listWidget);
    setLayout(vb);

    connect(edit, &QLineEdit::textChanged, [=](const QString &str){
        for(int i=0;i<listWidget->count();i++)
            listWidget->item(i)->setHidden(true);

        QList<QListWidgetItem *> items = listWidget->findItems(str, Qt::MatchContains);
        for(QListWidgetItem *item : items){
            item->setHidden(false);
        }
    });

    connect(listWidget, &QListWidget::itemDoubleClicked, [=](QListWidgetItem *item){
        int index = listWidget->row(item);
        QString url = "http://www02.eyny.com/" + animeList.at(index)["link"].toString();
        emit listWidget_itemDoubleClicked(url);
    });

    connect(slider, &QSlider::valueChanged, [=](int value){
        listWidget->setIconSize(QSize(value, value));
    });

    connect(button, &QPushButton::clicked, [=]{
        button->setEnabled(false);

        EynyWebControl e;
        int total = e.getAnimeTotalPage();
        processBar->setRange(0, total);
        processBar->setValue(0);
        connect(&e, &EynyWebControl::updateInformation, [=](int value){
            processBar->setValue(value);
        });

        QJsonObject allAnimeInfo = e.allAnimePageInfo();
        saveJson(allAnimeInfo);

        QList<QJsonObject> canAnalysisAnimeList;

        for(QString key : allAnimeInfo.keys()){
            QJsonObject animeInfo = allAnimeInfo.value(key).toObject();
            if(animeInfo["author"].toString() == "aglaia123"){
                canAnalysisAnimeList.push_back(animeInfo);
            }
        }

        setAnimeList(canAnalysisAnimeList);

        processBar->setValue(total);
        button->setEnabled(true);
    });

    connect(checkBox, &QCheckBox::toggled, [=](bool checked){
        if(checked){
            int i = 0;
            for(QJsonObject anime : animeList){
                QListWidgetItem *item = listWidget->item(i);
                eyny->getImage(anime["imageUrl"].toString(), [=](QImage image){
                    item->setIcon(QIcon(QPixmap::fromImage(image)));
                });
                i++;
            }
        }
        else{
            for(int i=0;i<listWidget->count();i++){
                QListWidgetItem *item = listWidget->item(i);
                item->setIcon(QIcon());
            }
        }
    });
}

void AnimeListWidget::setAnimeList(QList<QJsonObject> animeList)
{
    this->animeList = animeList;
    listWidget->clear();
    for(QJsonObject anime : animeList){
        //QImage img;
        //img.loadFromData(web->get(anime["imageUrl"].toString()));
        //listWidget->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(img)), anime["title"].toString()));
        listWidget->addItem(new QListWidgetItem(anime["title"].toString()));
    }

    /*int i = 0;
    for(QJsonObject anime : animeList){
        QListWidgetItem *item = listWidget->item(i);
        eyny->getImage(anime["imageUrl"].toString(), [=](QImage image){
            item->setIcon(QIcon(QPixmap::fromImage(image)));
        });
        i++;
    }*/

    /*QTimer::singleShot(1000, [=]{
        Asynchronous asynchronous;

        asynchronous.setFuntion([=, &asynchronous](){
            int i = 0;
            for(QJsonObject anime : animeList){

                QListWidgetItem *item = listWidget->item(i);
                item->setIcon(QIcon(QPixmap::fromImage(eyny->getImage(anime["imageUrl"].toString()))));
                i++;
            }
            asynchronous.finish();
        });

        asynchronous.start();
    });*/
}
