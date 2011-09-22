#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include "udpnetworksocket.h"
#include <QHostAddress>
#include <QAbstractSocket>

#define DATAGRAM_BUFFER_SIZE 1024

class UdpListener : public UdpNetworkSocket
{
    Q_OBJECT
public:
    explicit UdpListener(QObject *parent = 0);
    void stop();

private:
    bool _break;
    void run();

    void initSlots();

signals:
    void ReceivedNewData(QString data, QHostAddress addr);
    void BindError(int error, QString error_msg);

public slots:
    void stateChange(QAbstractSocket::SocketState socket_state);
    void reading();
};

#endif // UDPLISTENER_H
