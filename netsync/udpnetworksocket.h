#ifndef UDPNETWORKSOCKET_H
#define UDPNETWORKSOCKET_H

#include <QThread>
#include <QUdpSocket>
#include <QDateTime>

#define DATETIME_STRING_FORMAT "yyyy-MM-dd/hh-mm-ss/zzz"
#define BASKETPWD_UDP_PORT 35114

class UdpNetworkSocket : public QThread
{
    Q_OBJECT
public:
    explicit UdpNetworkSocket(QObject *parent = 0); //, bool bind = false);
    ~UdpNetworkSocket();

    void setBroadcastPort(quint16 port);
    quint16 broadcastPort() const;

protected:
    QUdpSocket *socket() const;

private:
    QUdpSocket *_socket;
    quint16 bcast_port;

signals:
    void errorBySend(QString errorMsg);
public slots:

};

#endif // UDPNETWORKSOCKET_H
