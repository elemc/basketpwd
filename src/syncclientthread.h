#ifndef SYNCCLIENTTHREAD_H
#define SYNCCLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QtXml>

class SyncClientThread : public QThread
{
    Q_OBJECT
public:
    explicit SyncClientThread(QTcpSocket *client, QObject *parent = 0);
protected:
    void run();
private:
    QTcpSocket *clientSocket;
    void parseDocument(QDomDocument &doc);
signals:
    void readError(QString errMsg, int errRow, int errColumn);
    void foundBasketHost(QString hostname, QString ip, quint16 port, QString id, QDateTime upd);
public slots:

};

#endif // SYNCCLIENTTHREAD_H
