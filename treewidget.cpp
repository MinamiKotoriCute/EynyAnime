#include "treewidget.h"

#include <QDebug>

TreeWidget::TreeWidget(QWidget *parent) : QTreeWidget(parent), hasCheckBox(false), changeFromUser(true)
{
    connect(this, &TreeWidget::itemChanged, this, &TreeWidget::treeWidget_itemChanged, Qt::DirectConnection);
}

QTreeWidgetItem* TreeWidget::addItem(QString text)
{
    QTreeWidgetItem *item = nullptr;
    for(int i=0;i<topLevelItemCount();i++){
        if(topLevelItem(i)->text(0).contains(text)){
            item = topLevelItem(i);
            break;
        }
    }

    if(item == nullptr){
        item = new QTreeWidgetItem(QStringList(text));
        addTopLevelItem(item);
        if(hasCheckBox)
            item->setCheckState(0, Qt::Unchecked);
    }

    return item;
}

QTreeWidgetItem *TreeWidget::addItem(QStringList text, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = nullptr;
    for(int i=0;i<parent->childCount();i++){
        if(parent->child(i)->text(0).contains(text.first())){
            bool isEqual = true;
            for(int j=1;j<text.size() && j<parent->child(i)->columnCount();j++){
                if(text.at(j) != parent->child(i)->text(j)){
                    isEqual = false;
                    break;
                }
            }
            if(!isEqual)
                continue;

            item = parent->child(i);
            break;
        }
    }

    if(item == nullptr){
        item = new QTreeWidgetItem(text);
        parent->addChild(item);
        if(hasCheckBox)
            item->setCheckState(0, Qt::Unchecked);
    }

    return item;
}

void TreeWidget::setHasCheckBox(bool value)
{
    hasCheckBox = value;
}

void TreeWidget::debug()
{
    for(int i=0;i<topLevelItemCount();i++){
        QTreeWidgetItem *parentItem = topLevelItem(i);

        debug(parentItem, 0);
    }
}

void TreeWidget::debug(QTreeWidgetItem *parentItem, int layer)
{
    QString tab;
    for(int i=0;i<layer;i++)
        tab += "\t";
    int n = -1;
    switch (parentItem->checkState(0)) {
    case Qt::Checked:
        n = 1;
        break;
    case Qt::Unchecked:
        n = 2;
        break;
    case Qt::PartiallyChecked:
        n = 3;
        break;
    default:
        break;
    }
    qDebug() << tab.toLocal8Bit().data() << n << "\t" << parentItem->text(0);

    for(int i=0;i<parentItem->childCount();i++){
        debug(parentItem->child(i), layer+1);
    }
}

void TreeWidget::treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;
    if (changeFromUser)
    {
        if (item->checkState(0) == Qt::PartiallyChecked) return;

        emit itemCheckStateChanged(item);

        changeFromUser = false;
        changeToParent = true;

        changeParents(item, item->checkState(0));
        changeChildren(item, item->checkState(0));
        changeFromUser = true;
    }
    else
    {
        if (changeToParent) changeParents(item, item->checkState(0));
        else changeChildren(item, item->checkState(0));
    }
}

void TreeWidget::changeParents(QTreeWidgetItem *item, Qt::CheckState state)
{
    QTreeWidgetItem *iParent = item->parent();
    if (iParent == 0)
    {
        changeToParent = false;
        return;
    }
    else
    {
        int i,k=iParent->childCount();
        for (i=0;i<k;i++)
        {
            if (iParent->child(i)->checkState(0) != state)
                break;
        }

        if (i==k){
            if(iParent->checkState(0) != state){
                emit itemCheckStateComingChanged(iParent);
                iParent->setCheckState(0, state);
                emit itemCheckStateChanged(iParent);
            }
        }
        else{
            if(iParent->checkState(0) != Qt::PartiallyChecked){
                emit itemCheckStateComingChanged(iParent);
                iParent->setCheckState(0, Qt::PartiallyChecked);
                emit itemCheckStateChanged(iParent);
            }
        }
    }
}

void TreeWidget::changeChildren(QTreeWidgetItem *item, Qt::CheckState state)
{
    int i,k=item->childCount();
    for (i=0;i<k;i++)
    {
        if(item->child(i)->checkState(0) != state){
            if(item->child(i)->checkState(0) != state){
                emit itemCheckStateComingChanged(item);
                item->child(i)->setCheckState(0, state);
                emit itemCheckStateChanged(item->child(i));
            }
        }
    }
}
