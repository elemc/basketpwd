#include "basketbaseitem.h"

BasketBaseItem::BasketBaseItem(BasketBaseItem *parentItem, QObject *parent) :
    QObject(parent)
{
    itemName        = QString();
    itemPassword    = QString();
    itemLogin       = QString();
    _parentItem     = parentItem;
    is_folder       = false;
    foldItem        = false;
}
BasketBaseItem::~BasketBaseItem()
{
    for ( int i = 0; i < childItems.count(); i++ ) {
        BasketBaseItem *it = childItems[i];
        delete it;
    }
    childItems.clear();
}
BasketBaseItem *BasketBaseItem::childItemAt(int row) const
{
    if( row >= 0 && row < childItems.count() ) {
        return childItems.value(row);
    }

    return 0;
}
BasketBaseItem *BasketBaseItem::parentItem() const
{
    return _parentItem;
}
int BasketBaseItem::childIndex(BasketBaseItem *child)
{
    if ( child )
        return childItems.indexOf(child);
    else
        return -1;
}
int BasketBaseItem::row()
{
    if ( _parentItem ) {
        return _parentItem->childIndex(this);
    }

    return -1;
}
int BasketBaseItem::childCount() const
{
    return childItems.count();
}

QString BasketBaseItem::name() const
{
    return itemName;
}
QString BasketBaseItem::login() const
{
    return itemLogin;
}
QString BasketBaseItem::password() const
{
    return itemPassword;
}
bool BasketBaseItem::isFolder() const
{
    return is_folder;
}

void BasketBaseItem::setFolder(QString name)
{
    is_folder = true;
    itemName = name;
}
void BasketBaseItem::setPassword(QString name, QString login, QString pwd)
{
    is_folder   = false;
    itemName    = name;
    itemLogin   = login;
    itemPassword= pwd;
}
void BasketBaseItem::addChild(BasketBaseItem *child)
{
    if ( is_folder )
        childItems.append(child);
}
void BasketBaseItem::insertChild(int i, BasketBaseItem *child)
{
    if ( is_folder )
        childItems.insert(i, child);
}
void BasketBaseItem::removeChild(BasketBaseItem *child)
{
    if ( is_folder )
#if QT_VERSION >= 0x040400
        childItems.removeOne(child);
#else
        childItems.removeAt(childItems.indexOf(child));
#endif
}
void BasketBaseItem::removeChildAt(int i)
{
    if ( is_folder )
        childItems.removeAt(i);
}

void BasketBaseItem::setName(QString name)
{
    itemName = name;
}
void BasketBaseItem::setLogin(QString login)
{
    if ( !isFolder() )
        itemLogin = login;
}
void BasketBaseItem::setEncryptedPassword(QString pwd)
{
    if ( !isFolder() )
        itemPassword = pwd;
}

void BasketBaseItem::sortList(QList<BasketBaseItem *> &list, Qt::SortOrder order)
{
    bool changes = false;
    do {
        changes = false;
        for ( int i = 0; i < list.size(); i++ ) {
            if ( i < list.size()-1 ) {
                QString next = list[i+1]->name();
                QString cur = list[i]->name();
                if ( order == Qt::AscendingOrder ) {
                    if ( next > cur ) {
                        list.swap(i, i+1);
                        changes = true;
                    }
                }
                else {
                    if ( next < cur ) {
                        list.swap(i, i+1);
                        changes = true;
                    }
                }
            }
        }
    } while ( changes );

}

void BasketBaseItem::sortChilds(Qt::SortOrder order) {
    QList<BasketBaseItem *> foldersList;
    QList<BasketBaseItem *> recordsList;
    foreach(BasketBaseItem *item, childItems) {
        if (item->isFolder()) {
            foldersList.append(item);
            item->sortChilds(order);
        }
        else
            recordsList.append(item);
    }

    childItems.clear();

    // сортируем
    sortList(foldersList, order);
    sortList(recordsList, order);

    foreach(BasketBaseItem *f, foldersList)
        childItems.append(f);

    foreach (BasketBaseItem *r, recordsList)
        childItems.append(r);

    foldersList.clear();
    recordsList.clear();
}
void BasketBaseItem::setFold(bool foldStatus, bool with_childs) {
    foldItem = foldStatus;

    if ( with_childs ) {
        foreach (BasketBaseItem *child, childItems) {
            if ( child->isFolder() )
                child->setFold(foldStatus, true);
        }
    }
}
bool BasketBaseItem::foldStatus() const {
    return foldItem;
}
