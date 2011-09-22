#ifndef SYNCCLIENT_H
#define SYNCCLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QDateTime>

class SyncClient : public QObject
{
    Q_OBJECT
public:
    explicit SyncClient(QObject *parent = 0);
    SyncClient(const QHostAddress &addr,
               quint16 port,
               QString clientID,
               const QDateTime &clientPos,
               QObject *parent = 0);
    SyncClient(const SyncClient &copy);

    void setAddress(QString sAddr);
    void setAddress(const QHostAddress &addr);
    QHostAddress address() const;

    void setPort(quint16 port);
    quint16 port() const;

    void setClientID(QString id);
    QString clientID() const;

    void setClientPosition(const QDateTime &dt);
    QDateTime clientPosition() const;

private:
    QHostAddress    _addr;
    quint16         _port;
    QString         _clientID;
    QDateTime       _clientPosition;
signals:

public slots:

};

#endif // SYNCCLIENT_H
