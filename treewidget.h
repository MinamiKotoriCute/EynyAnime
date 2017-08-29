#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

class TreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit TreeWidget(QWidget *parent = 0);

    QTreeWidgetItem* addItem(QString text);
    QTreeWidgetItem* addItem(QStringList text, QTreeWidgetItem *parent);

    void setHasCheckBox(bool value);

    void debug();
    void debug(QTreeWidgetItem *parentItem, int layer);

signals:
    void itemCheckStateChanged(QTreeWidgetItem *);
    void itemCheckStateComingChanged(QTreeWidgetItem *);
    void itemCheckStateChangedEnd();

public slots:

private:
    bool changeFromUser;
    bool changeToParent;
    bool hasCheckBox;

private slots:
    void treeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void changeParents(QTreeWidgetItem *item, Qt::CheckState state);
    void changeChildren(QTreeWidgetItem *item, Qt::CheckState state);
};

#endif // TREEWIDGET_H
