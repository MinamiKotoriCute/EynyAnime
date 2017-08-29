#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QListWidget>
#include <QTimer>

#include "downloadwidget.h"
#include "mainwebwidget.h"

#include "myitemdelegate.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //MainWebWidget ww;
    //ww.show();

    MainWindow w;
    w.show();

    DownloadWidget &d = DownloadWidget::Instance();
    d.show();

    return a.exec();
}
