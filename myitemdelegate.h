#ifndef MYITEMDELEGATE_H
#define MYITEMDELEGATE_H

#include <QObject>
#include <QItemDelegate>

class MyItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit MyItemDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // MYITEMDELEGATE_H
