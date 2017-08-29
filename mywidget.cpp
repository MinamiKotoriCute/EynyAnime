#include "mywidget.h"

#include <QDebug>

MyWidget::MyWidget(QWidget *parent) : QLabel(parent)
{

}

void MyWidget::enterEvent(QEvent *)
{
    qDebug() << "MyWidget::enterEvent";
}

void MyWidget::leaveEvent(QEvent *)
{
    qDebug() << "MyWidget::leaveEvent";
}
