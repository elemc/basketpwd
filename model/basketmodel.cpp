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

    request_client = 0;
}
BasketModel::~BasketModel()
{
    delete rootItem;

    if ( request_client ) {
        delete request_client;
        request_client = 0;
    }
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

    bool parse_res = parseDocument(doc);
    // added 0.4.4 (sync)
    if (parse_res) {
        if ( request_client ) {
            delete request_client;
            request_client = 0;
        }
        request_client = new SyncRequestThread();
        request_client->setSyncInfo(databaseIdentifier, lastDBModified);
        request_client->start();
    }

    return parse_res;
}
QByteArray BasketModel::modelDataToXML(bool encrypted)
{
    QDomDocument doc;
    QDomElement	root = doc.createElement( "basket-passwords" );
    root.setAttribute(QString("ident"), identifier());
    lastDBModified = QDateTime::currentDateTime();
    root.setAttribute(QString("modified"), lastDBModified.toString(DATE_TIME_FORMAT));

    doc.appendChild( root );
    for ( int i = 0; i < rootItem->childCount(); i++ ) {
        QDomElement child = convertBasketItemToDomElement(rootItem->childItemAt(i), doc);
        root.appendChild(child);
    }

    QByteArray clearBuffer;
    QBuffer clearFile ( &clearBuffer );
    if ( !clearFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return QByteArray();

    QTextStream clearStream(&clearFile);
    doc.save(clearStream, 4);
    clearFile.close();

    if ( !encrypted )
        return clearBuffer;

    BasketUtils butil;
    QByteArray encryptedBuffer = butil.crypt(clearBuffer, hash());

    return encryptedBuffer;
}

void BasketModel::setUpNewModel(QString pwd)
{
    beginResetModel();
    delete rootItem;
    rootItem = new BasketBaseItem(0, this);
    rootItem->setFolder(tr("корень"));

    hashPassword = pwd;

    lastDBModified = QDateTime::currentDateTime();
    databaseIdentifier = "generic";
    endResetModel();
}
bool BasketModel::changePassword(QString newPassword)
{
    beginResetModel();
    if ( hash().isEmpty() ) {
        hashPassword = newPassword;
        return true;
    }
    changeItemPassword(rootItem, newPassword);
    hashPassword = newPassword;
    endResetModel();

    emit modelDataChanged();

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
bool BasketModel::parseElementForDND(int row, const QModelIndex &parent, QDomElement element)
{
    if ( element.tagName() == "folder" ) {
        if ( !insertRow(row, parent, true) )
            return false;

        QModelIndex idx = index(row, 0, parent);
        BasketBaseItem *item;
        if ( !idx.isValid() )
            return false;
        item = static_cast<BasketBaseItem *>(idx.internalPointer());
        if ( !item )
            return false;
        item->setName(element.attribute("name", QString()));
        // парсим подчиненных
        for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling()) {
            if ( n.isElement() )
                parseElementForDND( rowCount(idx), idx, n.toElement() );
        }
    }
    else if ( element.tagName() == "item" ) {
        if ( !insertRow(row, parent, false) )
            return false;
        QModelIndex idx = index(row, 0, parent);
        BasketBaseItem *item;
        if ( !idx.isValid() )
            return false;
        item = static_cast<BasketBaseItem *>(idx.internalPointer());
        if ( !item )
            return false;

        QString name = element.attribute( "name", trUtf8("Без имени") );
        QString login, cipherPwd;

        // определяем логин
        QDomNodeList listLogin = element.elementsByTagName("login");
        QDomNodeList listPwd = element.elementsByTagName("pwd");
        if ( listLogin.count() > 0 )
            login = listLogin.at(0).toElement().text();
        if ( listPwd.count() > 0 )
            cipherPwd = listPwd.at(0).toElement().text();

        item->setName(name);
        item->setLogin(login);
        item->setEncryptedPassword(cipherPwd);
    }

    return true;
}

void BasketModel::setIdentifier(QString newIdent)
{
    databaseIdentifier = newIdent;
}
QString BasketModel::identifier() const
{
    return databaseIdentifier;
}
QDateTime BasketModel::lastModified() const
{
    return lastDBModified;
}
QString BasketModel::lastModifiedStr() const
{
    return lastDBModified.toString(DATE_TIME_FORMAT);
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
        emit modelDataChanged();
    }

}
QDomElement BasketModel::convertBasketItemToDomElement(BasketBaseItem *item, QDomDocument &doc) const
{
    QDomElement itemElem;
    if ( item->isFolder() ) {
        itemElem = doc.createElement("folder");
        itemElem.setAttribute("name", item->name());

        for ( int i = 0; i < item->childCount(); i++ ) {
            QDomElement childElem = convertBasketItemToDomElement(item->childItemAt(i), doc);
            itemElem.appendChild(childElem);
        }
    }
    else {
        itemElem = doc.createElement("item");
        itemElem.setAttribute("name", item->name());

        QDomElement login, pwd;
        login = doc.createElement("login");
        pwd = doc.createElement("pwd");

        QDomText tlogin, tpwd;
        tlogin = doc.createTextNode(item->login());
        tpwd = doc.createTextNode(item->password());

        login.appendChild(tlogin);
        pwd.appendChild(tpwd);

        itemElem.appendChild(login);
        itemElem.appendChild(pwd);
    }

    return itemElem;
}
QByteArray BasketModel::indexesToXML(const QModelIndexList &indexes) const
{
    QDomDocument doc;
    QDomElement	root = doc.createElement( "basket-passwords-items" );

    doc.appendChild( root );

    foreach(QModelIndex idx, indexes) {
        if ( !idx.isValid() )
            continue;
        if ( idx.column() != 0 )
            continue;

        BasketBaseItem *item = static_cast<BasketBaseItem *>(idx.internalPointer());
        if ( !item )
            continue;

        QDomElement child = convertBasketItemToDomElement(item, doc);
        root.appendChild(child);
    }

    QByteArray clearBuffer;
    QBuffer clearFile ( &clearBuffer );
    if ( !clearFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return QByteArray();

    QTextStream clearStream(&clearFile);
    doc.save(clearStream, 4);
    clearFile.close();

    return clearBuffer;
}
QString BasketModel::cleanPassword(QModelIndex idx) const
{
    if ( !idx.isValid() )
        return QString();

    BasketBaseItem *item = itemAtIndex(idx);
    if ( !item )
        return QString();

    QString encPwd = item->password();
    BasketUtils butil;
    QString cleanPwd = butil.decrypt(encPwd, hash());

    return cleanPwd;
}
QString BasketModel::login(QModelIndex idx) const
{
    BasketBaseItem *item = itemAtIndex(idx);
    if ( !item )
        return QString();

    QString login = item->login();

    return login;
}

// блок наследуемых методов
Qt::ItemFlags BasketModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (!index.isValid())
        return Qt::ItemIsDropEnabled | defaultFlags;

    BasketBaseItem *item = static_cast<BasketBaseItem *>(index.internalPointer());
    if ( !item )
        return 0;

    if ( (item->isFolder() && index.column() == 0) || (!item->isFolder() && index.column() != 2 ))
        defaultFlags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;

    if ( item->isFolder() )
        defaultFlags |= Qt::ItemIsDropEnabled;

    return defaultFlags;
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
        if ( index.column() == 0 ) {
            item->setName( value.toString() );
            emit modelDataChanged();
        }
        else
            return false;
    }
    else {
        switch( index.column() ) {
        case 0:
            item->setName(value.toString());
            emit modelDataChanged();
            break;
        case 1:
            item->setLogin(value.toString());
            emit modelDataChanged();
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

    parentItem->insertChild(row, newItem);

    endInsertRows();
    emit modelDataChanged();
    return true;
}
bool BasketModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    BasketBaseItem *pitem;

    if ( !parent.isValid() )
        pitem = rootItem;
    else
        pitem = static_cast<BasketBaseItem *>(parent.internalPointer());

    if ( !pitem )
        pitem = rootItem;

    pitem->removeChildAt(row);
    endRemoveRows();
    emit modelDataChanged();

    return true;
}
bool BasketModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = true;
    for ( int i = row; i < row+count; i++ )
        result &= removeRow(i, parent);

    return result;
}

// drag & drop
Qt::DropActions BasketModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
QStringList BasketModel::mimeTypes() const
{
    QStringList types;
    types << DRAG_AND_DROP_MIME;
    return types;
}
QMimeData *BasketModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData = indexesToXML(indexes);

    mimeData->setData(DRAG_AND_DROP_MIME, encodedData);
    return mimeData;
}
bool BasketModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column)
    if ( action == Qt::IgnoreAction )
        return true;

    if ( !data->hasFormat(DRAG_AND_DROP_MIME) )
        return false;


    BasketBaseItem *parentItem;
    int beginRow = row;
    if ( row == -1  && parent.isValid() )
        beginRow = rowCount(parent);
    else if ( row == -1 )
        beginRow = rowCount();

    if ( !parent.isValid() )
        parentItem = rootItem;
    else {
        parentItem = static_cast<BasketBaseItem *>(parent.internalPointer());
        if ( !parentItem )
            parentItem = rootItem;
    }

    QByteArray buf = data->data(DRAG_AND_DROP_MIME);
    QDomDocument doc;
    if ( !doc.setContent(buf) )
        return false;


    QDomElement rootElement = doc.documentElement();
    if ( rootElement.tagName() != "basket-passwords-items" )
        return false;

    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if ( n.isElement() )
            parseElementForDND( beginRow, parent, n.toElement() );
    }

    /*if ( action == Qt::CopyAction ) {

    }
    else if ( action == Qt::MoveAction ) {

    }*/
    emit modelDataChanged();
    return true;
}

// лишние методы, созданы для совместимости с предыдущими версиями
#if QT_VERSION < 0x040600
void BasketModel::beginResetModel()
{
}
void BasketModel::endResetModel()
{
    reset();
}
#endif
