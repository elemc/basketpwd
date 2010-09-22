#ifndef BASKETBASEITEM_H
#define BASKETBASEITEM_H

#include <QObject>

class BasketBaseItem : public QObject
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

    void setFolder(QString name);
    void setPassword(QString name, QString login, QString pwd);
    void setName(QString name);
    void setLogin(QString login);
    void setEncryptedPassword(QString pwd);
    void addChild(BasketBaseItem *child);
    void insertChild(int i, BasketBaseItem *child);
    void removeChild(BasketBaseItem *child);
    void removeChildAt(int i);

private:
    BasketBaseItem *_parentItem;

    QString itemName;
    QString itemLogin;
    QString itemPassword;
    bool is_folder;

    QList<BasketBaseItem *> childItems;

signals:

public slots:

};

#endif // BASKETBASEITEM_H
