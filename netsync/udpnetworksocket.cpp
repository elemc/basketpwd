#include "udpnetworksocket.h"

UdpNetworkSocket::UdpNetworkSocket(QObject *parent) : //, bool bind) :
    QThread(parent)
{
//    if ( bind )
//        _socket = new QUdpSocket(this);
//    else {
    _socket = new QUdpSocket;
    _socket->moveToThread(this);
//    }

    bcast_port = BASKETPWD_UDP_PORT;
}
UdpNetworkSocket::~UdpNetworkSocket()
{
    delete _socket;
}

void UdpNetworkSocket::setBroadcastPort(quint16 port)
{
    bcast_port = port;
}
quint16 UdpNetworkSocket::broadcastPort() const
{
    return bcast_port;
}
QUdpSocket *UdpNetworkSocket::socket() const
{
    return _socket;
}
