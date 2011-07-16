#ifndef BASKETMODEL_H
#define BASKETMODEL_H

#include <QAbstractItemModel>
#include <QtXml>
#include <QDateTime>
#include <QIcon>
#include <QBrush>
#include "basketbaseitem.h"
#include "../src/basketutils.h"

#define DATE_TIME_FORMAT QString("yyyy-MM-dd/hh:mm")
#define DRAG_AND_DROP_MIME QString("application/basketpwd.records")

class BasketModel : public QAbstractItemModel
{
Q_OBJECT
public:
    explicit BasketModel(QObject *parent = 0);
    ~BasketModel();

    bool setModelData(QByteArray &data, QString pwd = QString(), bool isEncryptedData = true);
    void setUpNewModel(QString pwd = QString());
    bool changePassword(QString newPassword);
    void setIdentifier(QString newIdent);

    QByteArray modelDataToXML(bool encrypted = true);
    QString identifier() const;
    QDateTime lastModified() const;
    QString lastModifiedStr() const;
    QString hash() const;
    QString cleanPassword(QModelIndex idx) const;
    QString login(QModelIndex idx) const;
    void setPassword(QModelIndex index, QString newPassword);
    void setShowPasswords(bool statusShow);
    bool statusShowPasswords() const;
    bool indexIsFolder(QModelIndex idx = QModelIndex()) const;
    BasketBaseItem *itemAtIndex(QModelIndex &index) const;

    // наследуемые методы
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    // наследуемые методы редактирования
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex(), bool isFolder = false);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent = QModelIndex());

protected:
#if QT_VERSION < 0x040600
    void beginResetModel();
    void endResetModel();
#endif

private:
    BasketBaseItem *rootItem;
    bool showPasswords;

    QString     databaseIdentifier;
    QDateTime   lastDBModified;
    QString     hashPassword;

    bool parseDocument(QDomDocument &doc);
    bool parseElement(BasketBaseItem *parentItem, QDomElement element);
    bool parseElementForDND(int row, const QModelIndex &parent, QDomElement element);
    bool changeItemPassword(BasketBaseItem *item, QString newPassword);
    QDomElement convertBasketItemToDomElement(BasketBaseItem *item, QDomDocument &doc) const;
    QByteArray indexesToXML(const QModelIndexList &indexes) const;
//    void softReset();
//    void softResetItem(BasketBaseItem *item);

    // Декорации
    QIcon recordIcon;
    QIcon folderIcon;
    QIcon folderCloseIcon;
    Qt::SortOrder mainSortOrder;

signals:
    void modelDataChanged();
    void ThisIndexIsFold(const QModelIndex &index) const;

public slots:
    void setFoldIndex(QModelIndex idx = QModelIndex());
    void setUnFoldIndex(QModelIndex idx = QModelIndex());
    void setFoldAll();
    void setUnFoldAll();
};

#endif // BASKETMODEL_H
