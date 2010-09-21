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

class BasketModel : public QAbstractItemModel
{
Q_OBJECT
public:
    explicit BasketModel(QObject *parent = 0);
    ~BasketModel();

    bool setModelData(QByteArray &data, QString pwd = QString(), bool isEncryptedData = true);
    void setUpNewModel(QString pwd = QString());
    bool changePassword(QString newPassword);

    QByteArray storeData();
    QString identifier() const;
    QDateTime lastModified() const;
    QString hash() const;
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

    // наследуемые методы редактирования
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex(), bool isFolder = false);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    // копирование/перемещение
    Qt::DropActions supportedDropActions() const;
    Qt::DropActions supportedDragActions() const;

private:
    BasketBaseItem *rootItem;
    bool showPasswords;

    QString     databaseIdentifier;
    QDateTime   lastDBModified;
    QString     hashPassword;

    bool parseDocument(QDomDocument &doc);
    bool parseElement(BasketBaseItem *parentItem, QDomElement element);
    bool changeItemPassword(BasketBaseItem *item, QString newPassword);

    // Декорации
    QIcon recordIcon;
    QIcon folderIcon;
    QIcon folderCloseIcon;

signals:

public slots:

};

#endif // BASKETMODEL_H
