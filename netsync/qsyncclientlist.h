#ifndef QSYNCCLIENTLIST_H
#define QSYNCCLIENTLIST_H

#include <QList>
#include "syncclient.h"

typedef QListIterator<SyncClient> QSyncClientListIterator;
typedef QMutableListIterator<SyncClient> QMutableSyncClientIterator;

class QSyncClientList : public QList<SyncClient>
{
public:
    inline QSyncClientList() {}
    inline explicit QSyncClientList(const QSyncClientList &i) : QList<SyncClient>(i) { }
    inline QSyncClientList(const QList<SyncClient> &i) : QList<SyncClient>(i) { }

    bool contains(const SyncClient &t) const;
    void append(const SyncClient &t);

signals:

public slots:

};

#endif // QSYNCCLIENTLIST_H
