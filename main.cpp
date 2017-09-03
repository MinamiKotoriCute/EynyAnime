#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QListWidget>
#include <QTimer>

#include "downloadwidget.h"
#include "homewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    DownloadWidget &d = DownloadWidget::Instance();
    d.show();

    //HomeWidget h;
    //h.show();

    return a.exec();
}
