#include "udplistener.h"

UdpListener::UdpListener(QObject *parent) :
    UdpNetworkSocket(parent)
{
    _break = false;
    initSlots();
}
void UdpListener::run()
{
    bool result = socket()->bind(QHostAddress::Any, broadcastPort());

    if ( !result ) {
        emit BindError(socket()->error(), socket()->errorString());
        return;
    }

    while(!_break) {
        socket()->waitForReadyRead(100);
    }
}
void UdpListener::stop()
{
    _break = true;
    socket()->abort();
    socket()->close();
    wait();
    emit terminated();
    emit finished();
}

void UdpListener::initSlots()
{
    connect(socket(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChange(QAbstractSocket::SocketState)));
    connect(socket(), SIGNAL(readyRead()),this, SLOT(reading()));
}

// Slots
void UdpListener::stateChange(QAbstractSocket::SocketState socket_state)
{
    Q_UNUSED(socket_state)
}
void UdpListener::reading()
{
    QByteArray new_data;
    char *buf = new char[DATAGRAM_BUFFER_SIZE];
    QHostAddress client_addr;
    while ( socket()->hasPendingDatagrams() ) {
        int read_size = socket()->readDatagram(buf, DATAGRAM_BUFFER_SIZE, &client_addr);
        if ( read_size <= 0 )
            break;
        else {
            QByteArray new_data_part(buf, read_size);
            new_data += new_data_part;
        }

        if ( read_size < DATAGRAM_BUFFER_SIZE )
            break;
    }
    delete [] buf;

    if ( !new_data.isNull() && !client_addr.isNull() )
        emit ReceivedNewData(QString(new_data), client_addr);
}
