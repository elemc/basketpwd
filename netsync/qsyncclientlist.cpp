#include "qsyncclientlist.h"

/*QSyncClientList::QSyncClientList() :
    QList<SyncClient>()
{
}*/

bool QSyncClientList::contains(const SyncClient &t) const
{
    QList<SyncClient>::const_iterator iter;
    bool result = false;

    for ( iter = begin(); iter != end(); ++iter) {
        SyncClient i = *iter;
        if (t == i )
            result = true;
    }

    return result;
}

void QSyncClientList::append(const SyncClient &t)
{
    if ( !contains(t) ) {
        QList<SyncClient>::append(t);
    }
}
