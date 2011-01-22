#include "syncthread.h"

SyncThread::SyncThread(QObject *parent) :
    QThread(parent)
{
    main_server = 0;
}
void SyncThread::run()
{
    main_server = new QTcpServer();
    server_addr = QHostAddress(QHostAddress::Any);
    server_port = 35310;

    connect(main_server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    bool result = main_server->listen(server_addr, server_port);
    if (!result) {
        emit serverStartFail(main_server->errorString());
        return;
    }
    main_server->moveToThread(this);
    exec();
}
void SyncThread::exit(int retcode)
{
    Q_UNUSED(retcode)
    main_server->close();
    quit();
}
void SyncThread::clientConnected()
{
    QTcpSocket *sock = main_server->nextPendingConnection();
    SyncClientThread *sct = new SyncClientThread(sock);
    connect(sct, SIGNAL(foundBasketHost(QString,QString,quint16,QString,QDateTime)), this, SLOT(foundedBasketHost(QString,QString,quint16,QString,QDateTime)));
    //sock->moveToThread(sct);
    sct->start();

}
void SyncThread::foundedBasketHost(QString hostname, QString ip, quint16 port, QString id, QDateTime upd)
{
    emit foundBasketHost(hostname, ip, port, id, upd);
}
