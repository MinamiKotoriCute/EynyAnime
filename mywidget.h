#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QLabel>

class MyWidget : public QLabel
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // MYWIDGET_H
