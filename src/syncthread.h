#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include "src/syncclientthread.h"

class SyncThread : public QThread
{
    Q_OBJECT
public:
    explicit SyncThread(QObject *parent = 0);
    void exit(int retcode = 0);
protected:
    void run();
private:
    QTcpServer *main_server;
    QHostAddress server_addr;
    quint16 server_port;

signals:
    void serverStartFail(QString errorMessage);
    void foundBasketHost(QString hostname, QString ip, quint16 port, QString id, QDateTime upd);

private slots:
    void clientConnected();
    void foundedBasketHost(QString hostname, QString ip, quint16 port, QString id, QDateTime upd);

};

#endif // SYNCTHREAD_H
