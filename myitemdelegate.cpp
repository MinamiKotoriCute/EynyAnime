#include "myitemdelegate.h"

#include <QDebug>
#include <QLabel>
#include <QPainter>

#include "mywidget.h"

MyItemDelegate::MyItemDelegate(QObject *parent) : QItemDelegate(parent)
{

}

QWidget *MyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    MyWidget *w = new MyWidget(parent);
    w->setText(index.data().toString());

    qDebug() << "MyItemDelegate::createEditor" << index.data();

    return w;
}

void MyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if(option.state & QStyle::State_MouseOver){
        painter->fillRect(option.rect, QColor(255,0,0));
    }
    else{
        painter->fillRect(option.rect, QColor(0,255,0));
    }
    //painter->drawText(option.rect, index.data().toString());
    qDebug() << qrand();

    painter->restore();

    QItemDelegate::paint(painter, option, index);
}

