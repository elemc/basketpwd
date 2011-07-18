#include "firstnetworksender.h"

FirstNetworkSender::FirstNetworkSender(QObject *parent) :
    QThread(parent)
{
    socket = new QUdpSocket;
    id = QString();
    bcast_port = BASKETPWD_UDP_PORT;

    socket->moveToThread(this);
}
FirstNetworkSender::~FirstNetworkSender()
{
    delete socket;
}

void FirstNetworkSender::setId(QString _id)
{
    id = _id;
}
void FirstNetworkSender::setDate(QDateTime _dateTime)
{
    dateTime = _dateTime;
}
void FirstNetworkSender::setBroadcastPort(quint16 port)
{
    bcast_port = port;
}

void FirstNetworkSender::run()
{

    QString sign = QString("basketpwd-great:%1:%2").arg(id).arg(dateTime.toString(DATETIME_STRING_FORMAT));
    QByteArray dgram = sign.toUtf8();
    if ( dgram.size() > 8192 ) {
        dgram.resize(8192);
    }

    qint64 result = socket->writeDatagram(dgram, QHostAddress::Broadcast, bcast_port);
    if ( result < 0 ) {
        emit errorBySend(socket->errorString());
    }
}
