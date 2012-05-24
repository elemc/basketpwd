#ifndef BASKETBASEITEM_H
#define BASKETBASEITEM_H

#include <QObject>
#include "basketpwd-libs_global.h"

class BASKETPWDLIBSSHARED_EXPORT BasketBaseItem : public QObject
{
Q_OBJECT
public:
    explicit BasketBaseItem(BasketBaseItem *parentItem, QObject *parent = 0);
    //BasketBaseItem()
    ~BasketBaseItem();

    BasketBaseItem *childItemAt(int row) const;
    BasketBaseItem *parentItem() const;
    int childIndex(BasketBaseItem *child = 0);
    int row();
    int childCount() const;

    QString name() const;
    QString login() const;
    QString password() const;
    bool isFolder() const;
    bool foldStatus() const;

    void setFolder(QString name);
    void setPassword(QString name, QString login, QString pwd);
    void setName(QString name);
    void setLogin(QString login);
    void setEncryptedPassword(QString pwd);
    void addChild(BasketBaseItem *child);
    void insertChild(int i, BasketBaseItem *child);
    void removeChild(BasketBaseItem *child);
    void removeChildAt(int i);

    void sortChilds(Qt::SortOrder order = Qt::AscendingOrder);
    void setFold(bool foldStatus = true, bool with_childs = false);
    void setPrimaryItem(bool status = false);
    bool isPrimary() const;

    QList<BasketBaseItem *>primaryList();

private:
    BasketBaseItem *_parentItem;

    QString itemName;
    QString itemLogin;
    QString itemPassword;
    bool is_folder;
    bool foldItem;
    bool primaryItem;

    QList<BasketBaseItem *> childItems;
    void privatePrimaryList(QList<BasketBaseItem *> &list, BasketBaseItem *_parent);

    void sortList(QList<BasketBaseItem *> &list, Qt::SortOrder order = Qt::AscendingOrder);

signals:

public slots:

};

#endif // BASKETBASEITEM_H
