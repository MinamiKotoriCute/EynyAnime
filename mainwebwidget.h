#ifndef MAINWEBWIDGET_H
#define MAINWEBWIDGET_H

#include <QWidget>
#include <QJsonObject>

class EynyWebControl;
class QLabel;

class MainWebWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWebWidget(QWidget *parent = 0);

    QJsonObject saveJson();
    void loadJson(QJsonObject json);

signals:

public slots:

private:
    enum JsonKeys{
        LastUpdateTime
    };

    EynyWebControl *web;

    QLabel *lastUpdateTimeLabel;
};

#endif // MAINWEBWIDGET_H
