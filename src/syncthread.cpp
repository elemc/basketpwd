#include "syncthread.h"

SyncThread::SyncThread(QObject *parent) :
    QThread(parent)
{
    main_server = new QTcpServer(this);
    server_addr = QHostAddress(QHostAddress::Any);
    server_port = 35310;

    connect(main_server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
}
void SyncThread::run()
{
    bool result = main_server->listen(server_addr, server_port);
    if (!result)
        emit serverStartFail(main_server->errorString());
    else
        exec();
}
void SyncThread::exit(int retcode)
{
    Q_UNUSED(retcode)
    main_server->close();
}
void SyncThread::clientConnected()
{
    QTcpSocket *sock = main_server->nextPendingConnection();
    SyncClientThread *sct = new SyncClientThread(sock, this);
    connect(sct, SIGNAL(foundBasketHost(QString,QString,quint16,QString,QDateTime)), this, SLOT(foundedBasketHost(QString,QString,quint16,QString,QDateTime)));
    sct->start();

}
void SyncThread::foundedBasketHost(QString hostname, QString ip, quint16 port, QString id, QDateTime upd)
{
    emit foundBasketHost(hostname, ip, port, id, upd);
}
