#ifndef FIRSTNETWORKSENDER_H
#define FIRSTNETWORKSENDER_H

//#include <QThread>
//#include <QUdpSocket>
//#include <QDateTime>
//#include "udpnetworksocket.h"

class FirstNetworkSender : public UdpNetworkSocket
{
    Q_OBJECT
public:
    explicit FirstNetworkSender(QObject *parent = 0);

    void setId(QString _id);
    void setDate(QDateTime _dateTime);

private:
    QString id;
    QDateTime dateTime;

    void run();

signals:

public slots:

};

#endif // FIRSTNETWORKSENDER_H
