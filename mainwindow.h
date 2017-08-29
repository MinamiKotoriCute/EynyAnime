#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "eynywebcontrol.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);

private:
    void readSettings();
    void writeSettings();

    EynyWebControl *eyny;
};

#endif // MAINWINDOW_H
