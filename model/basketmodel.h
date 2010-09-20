#ifndef BASKETMODEL_H
#define BASKETMODEL_H

#include <QAbstractItemModel>
#include <QtXml>
#include <QDateTime>
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
    QString identifier() const;
    QDateTime lastModified() const;

    // наследуемые методы
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

private:
    BasketBaseItem *rootItem;
    bool showPasswords;

    QString     databaseIdentifier;
    QDateTime   lastDBModified;

    bool parseDocument(QDomDocument &doc);
    bool parseElement(BasketBaseItem *parentItem, QDomElement element);

signals:

public slots:

};

#endif // BASKETMODEL_H
