#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QListWidget>
#include <QTimer>

#include "downloadwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    DownloadWidget &d = DownloadWidget::Instance();
    d.show();

    return a.exec();
}
