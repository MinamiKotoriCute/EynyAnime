#include "mainwebwidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QSettings>

#include <asynchronous.h>

#include "eynywebcontrol.h"

const char* jsonKeysName[] = {
    "Last Update Time"
};

MainWebWidget::MainWebWidget(QWidget *parent) :
    QWidget(parent), web(new EynyWebControl)
{
    lastUpdateTimeLabel = new QLabel("never update");
    QLabel *totalPageLabel = new QLabel("wait");
    QPushButton *updateButton = new QPushButton("update");
    QLineEdit *edit = new QLineEdit("10");

    QVBoxLayout *vb = new QVBoxLayout;
    vb->addWidget(lastUpdateTimeLabel);
    vb->addWidget(totalPageLabel);
    setLayout(vb);

    connect(updateButton, &QPushButton::clicked, [=]{
        //QJsonObject;
    });

    QTimer::singleShot(0, [=]{
        int totalPage = web->getAnimeTotalPage();
        totalPageLabel->setText(QString::asprintf("total page: %d", totalPage));
        edit->setValidator(new QIntValidator(1, totalPage));
    });
}

QJsonObject MainWebWidget::saveJson()
{
    QJsonObject retValue;
    retValue[jsonKeysName[LastUpdateTime]] = lastUpdateTimeLabel->text();

    return retValue;
}

void MainWebWidget::loadJson(QJsonObject json)
{
    lastUpdateTimeLabel->setText(json[jsonKeysName[LastUpdateTime]].toString());
}
