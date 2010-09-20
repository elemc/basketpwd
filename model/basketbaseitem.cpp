#include "basketbaseitem.h"

BasketBaseItem::BasketBaseItem(BasketBaseItem *parentItem, QObject *parent) :
    QObject(parent)
{
    itemName        = QString();
    itemPassword    = QString();
    itemLogin       = QString();
    _parentItem     = parentItem;
    is_folder       = false;
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
