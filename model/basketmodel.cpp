#include "basketmodel.h"

BasketModel::BasketModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem = new BasketBaseItem(0, this);
    rootItem->setFolder(tr("корень"));
    showPasswords = false;

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

    endResetModel();
    return parseDocument(doc);
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

// блок наследуемых методов
Qt::ItemFlags BasketModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
         return 0;

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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

    if ( role == Qt::DisplayRole ) {
        switch(index.column()) {
        case 0:
            return item->name();
            break;
        case 1:
            return item->login();
            break;
        case 2:
            if ( showPasswords )
                return item->password();
            else if ( !item->isFolder() )
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
