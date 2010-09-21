#include "basketmodel.h"

BasketModel::BasketModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem        = new BasketBaseItem(0, this);
    rootItem->setFolder(tr("корень"));
    showPasswords   = false;
    hashPassword    = QString();

    // иконки
    recordIcon = QIcon(":/images/recordicon");
    folderIcon = QIcon(":/images/foldericon");
    folderCloseIcon = QIcon(":/images/foldercloseicon");

}
BasketModel::~BasketModel()
{
    delete rootItem;
}

bool BasketModel::setModelData(QByteArray &data, QString pwd, bool isEncryptedData)
{
    beginResetModel();
    QByteArray rawData = data;
    if ( isEncryptedData ) {
        BasketUtils butil;
        rawData = butil.decrypt(data, pwd);
    }
    QDomDocument doc;
    QString errMsg;
    int errLine, errCol;

    bool readResult = doc.setContent( rawData, &errMsg, &errLine, &errCol );
    if ( !readResult ) {
        endResetModel();
        return false;
    }

    hashPassword = pwd;
    endResetModel();
    return parseDocument(doc);
}
void BasketModel::setUpNewModel(QString pwd)
{
    beginResetModel();
    delete rootItem;
    rootItem = new BasketBaseItem(0, this);
    rootItem->setFolder(tr("корень"));

    hashPassword = pwd;
    endResetModel();
}
bool BasketModel::changePassword(QString newPassword)
{
    beginResetModel();
    if ( hash().isEmpty() )
        return true;
    changeItemPassword(rootItem, newPassword);
    hashPassword = newPassword;
    endResetModel();

    return true;
}
bool BasketModel::changeItemPassword(BasketBaseItem *item, QString newPassword)
{
    if ( item->isFolder() ) {
        bool result = true;
        for ( int i = 0; i < item->childCount(); i++ ) {
            BasketBaseItem *child = item->childItemAt(i);
            result &= changeItemPassword(child, newPassword);
        }

        return result;
    }
    else {

        BasketUtils butil;
        QString clearPwd = butil.decrypt(item->password(), hash() );
        QString newPwd = butil.crypt(clearPwd, newPassword);
        item->setEncryptedPassword(newPwd);
    }

    return true;
}
bool BasketModel::indexIsFolder(QModelIndex idx) const
{
    if ( !idx.isValid() )
        return true;

    if ( BasketBaseItem *item = static_cast<BasketBaseItem *>(idx.internalPointer()) )
        return item->isFolder();

    return false;
}

// приваты обработки
bool BasketModel::parseDocument(QDomDocument &doc)
{
    QDomElement rootElement = doc.documentElement();
    if ( rootElement.tagName() != "basket-passwords" )
        return false;

    databaseIdentifier  = rootElement.attribute(QString("ident"), QString("default"));
    lastDBModified        = QDateTime::fromString(rootElement.attribute(QString("modified"), QDateTime::currentDateTime().toString(DATE_TIME_FORMAT)), DATE_TIME_FORMAT);

    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if ( n.isElement() )
            parseElement ( rootItem, n.toElement() );
    }

    return true;

}
bool BasketModel::parseElement(BasketBaseItem *parentItem, QDomElement element)
{
    //Смотрим, что это папка или запись
    if ( element.tagName() == "folder" ) {
        QString folderName = element.attribute( "name", trUtf8("Без имени") );
        BasketBaseItem *fold = new BasketBaseItem( parentItem, this );
        fold->setFolder(folderName);

        // добавляем папочку в дерево
        parentItem->addChild(fold);

        // парсируем, все, что внутри
        for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
        {
            if ( n.isElement() )
                parseElement ( fold, n.toElement() );
        }
    }
    else if ( element.tagName() == "item" ) {
        QString name = element.attribute( "name", trUtf8("Без имени") );
        QString login, cipherPwd;

        // определяем логин
        QDomNodeList listLogin = element.elementsByTagName("login");
        QDomNodeList listPwd = element.elementsByTagName("pwd");
        if ( listLogin.count() > 0 )
            login = listLogin.at(0).toElement().text();
        if ( listPwd.count() > 0 )
            cipherPwd = listPwd.at(0).toElement().text();

        BasketBaseItem *itemPwd = new BasketBaseItem( parentItem, this );
        itemPwd->setPassword(name, login, cipherPwd);

        // добавляем запись в дерево
        parentItem->addChild(itemPwd);
    }

    return true;
}
QString BasketModel::identifier() const
{
    return databaseIdentifier;
}
QDateTime BasketModel::lastModified() const
{
    return lastDBModified;
}
BasketBaseItem *BasketModel::itemAtIndex(QModelIndex &index) const
{
    if ( !index.isValid() )
        return 0;

    BasketBaseItem *item = static_cast<BasketBaseItem *>(index.internalPointer());
    if ( !item )
        return 0;

    return item;
}
QString BasketModel::hash() const
{
    return hashPassword;
}
void BasketModel::setShowPasswords(bool statusShow)
{
    showPasswords = statusShow;
    reset();
}
bool BasketModel::statusShowPasswords() const
{
    return showPasswords;
}
void BasketModel::setPassword(QModelIndex idx, QString newPassword)
{
    if ( !idx.isValid() )
        return;

    if ( BasketBaseItem *item = static_cast<BasketBaseItem *>(idx.internalPointer()) ) {
        BasketUtils butil;
        item->setEncryptedPassword(butil.crypt(newPassword, hashPassword));
        emit dataChanged(index(idx.row(), 0, idx.parent()), index(idx.row(), 2, idx.parent()));
    }

}

// блок наследуемых методов
Qt::ItemFlags BasketModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
         return 0;

    Qt::ItemFlags selFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    Qt::ItemFlags editFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    BasketBaseItem *item = static_cast<BasketBaseItem *>(index.internalPointer());
    if ( !item )
        return 0;

    if ( (item->isFolder() && index.column() > 0) ||
         (!item->isFolder() && index.column() == 2) )
        return selFlags;
    return editFlags;
}
int BasketModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}
QVariant BasketModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch (section) {
            case 0:
                return tr("Наименование");
                break;
            case 1:
                return tr("Имя (логин)");
                break;
            case 2:
                return tr("Пароль");
                break;
            default:
                return QString();
                break;
            }
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}
QModelIndex BasketModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    BasketBaseItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<BasketBaseItem *>(parent.internalPointer());
    BasketBaseItem *childItem = parentItem->childItemAt(row);
    if ( childItem )
        return createIndex(row, column, childItem);

    return QModelIndex();
}
QModelIndex BasketModel::parent(const QModelIndex &child) const
{
    if ( !child.isValid() )
        return QModelIndex();

    BasketBaseItem *childItem = static_cast<BasketBaseItem *>(child.internalPointer());
    BasketBaseItem *parentItem  = childItem->parentItem();

    if ( !parentItem || parentItem == rootItem )
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
QVariant BasketModel::data(const QModelIndex &index, int role) const
{
    if ( !index.isValid() )
        return QVariant();

    BasketBaseItem *item = static_cast<BasketBaseItem *>(index.internalPointer());
    if ( !item )
        return QVariant();

    if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
        switch(index.column()) {
        case 0:
            return item->name();
            break;
        case 1:
            return item->login();
            break;
        case 2:
            if ( showPasswords ) {
                BasketUtils butil;
                QString clearPassword = butil.decrypt(item->password(), hash());
                return clearPassword;
            }
            else if ( !item->isFolder() && role == Qt::DisplayRole )
                return tr("* * *");
            break;
        default:
            break;
        }
    }
    else if ( role == Qt::DecorationRole && index.column() == 0 ) {
        if ( item->isFolder() )
            return folderIcon;
        else
            return recordIcon;
    }
    else if ( role == Qt::TextColorRole ) {
        QBrush blue_brush;
        blue_brush.setColor(Qt::darkBlue);
        if ( !item->isFolder() )
            return blue_brush;
    }


    return QVariant();
}
int BasketModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.column() > 0 )
        return 0;

    BasketBaseItem *item;
    if ( !parent.isValid() )
        item = rootItem;
    else
        item = static_cast<BasketBaseItem *>(parent.internalPointer());

    return item->childCount();
}

// наследуемые редактирование
bool BasketModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( !index.isValid() )
        return false;

    BasketBaseItem *item = static_cast<BasketBaseItem *>(index.internalPointer());
    if ( !item )
        return false;

    if ( role != Qt::EditRole )
        return QAbstractItemModel::setData(index, value, role);

    if ( item->isFolder() ) {
        if ( index.column() == 0 )
            item->setName( value.toString() );
        else
            return false;
    }
    else {
        switch( index.column() ) {
        case 0:
            item->setName(value.toString());
            break;
        case 1:
            item->setLogin(value.toString());
            break;
        case 2:
            return false;
        }
    }

    return true;
}
bool BasketModel::insertRow(int row, const QModelIndex &parent, bool isFolder)
{
    beginInsertRows(parent, row, row);
    //return insertRows(row, 1, parent, isFolder);
    BasketBaseItem *parentItem;
    if ( parent.isValid() )
        parentItem = static_cast<BasketBaseItem *>(parent.internalPointer());
    else
        parentItem = rootItem;
    if ( !parentItem )
        parentItem = rootItem;

    BasketBaseItem *newItem = new BasketBaseItem(parentItem, this);
    if ( !isFolder )
        newItem->setPassword(tr("Новый пароль"), QString(), QString());
    else
        newItem->setFolder(tr("Новая папка"));

    parentItem->addChild(newItem);

    endInsertRows();
    return true;
}
