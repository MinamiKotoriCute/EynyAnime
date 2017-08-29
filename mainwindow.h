#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "eynywebcontrol.h"
#include "animelistwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private:
    void readSettings();
    void writeSettings();

    void loadAnimeData();

    EynyWebControl *eyny;
    AnimeListWidget *animeListWidget;
};

#endif // MAINWINDOW_H
