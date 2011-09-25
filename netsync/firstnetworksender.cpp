#include "firstnetworksender.h"

FirstNetworkSender::FirstNetworkSender(QObject *parent) :
    UdpNetworkSocket(parent)
{
    id = QString();
}

void FirstNetworkSender::setId(QString _id)
{
    id = _id;
}
void FirstNetworkSender::setDate(QDateTime _dateTime)
{
    dateTime = _dateTime;
}

void FirstNetworkSender::run()
{
    QString sign = QString("basketpwd-discover-great:%1:%2:udp=%3:tcp=%4").
            arg(id).
            arg(dateTime.toString(DATETIME_STRING_FORMAT)).
            arg(broadcastPort()).
            arg(broadcastPort());
    QByteArray dgram = sign.toUtf8();
    if ( dgram.size() > 8192 ) {
        dgram.resize(8192);
    }

    qint64 result = socket()->writeDatagram(dgram, QHostAddress::Broadcast, broadcastPort());
    if ( result < 0 ) {
        emit errorBySend(socket()->errorString());
    }
}
