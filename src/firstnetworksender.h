#ifndef FIRSTNETWORKSENDER_H
#define FIRSTNETWORKSENDER_H

#include <QThread>
#include <QUdpSocket>
#include <QDateTime>

#define DATETIME_STRING_FORMAT "yyyy-MM-dd/hh-mm-ss/zzz"
#define BASKETPWD_UDP_PORT 35114

class FirstNetworkSender : public QThread
{
    Q_OBJECT
public:
    explicit FirstNetworkSender(QObject *parent = 0);
    ~FirstNetworkSender();
    void setId(QString _id);
    void setDate(QDateTime _dateTime);
    void setBroadcastPort(quint16 port);

private:
    QUdpSocket *socket;
    QString id;
    QDateTime dateTime;
    quint16 bcast_port;

    void run();

signals:
    void errorBySend(QString errorMsg);
public slots:

};

#endif // FIRSTNETWORKSENDER_H
