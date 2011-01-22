#include "syncrequestthread.h"

SyncRequestThread::SyncRequestThread(QObject *parent) :
    QThread(parent)
{
}
QDomElement SyncRequestThread::createElement(QDomDocument &doc, QString name, QString value)
{
    QDomElement elem = doc.createElement(name);
    QDomText elem_val= doc.createTextNode(value);
    elem.appendChild(elem_val);

    return elem;
}
void SyncRequestThread::setSyncInfo(QString id, QDateTime dt)
{
    sync_id = id;
    sync_date = dt;
}

void SyncRequestThread::run()
{
    // cooking xml request
    QDomDocument doc;
    QDomElement	root = doc.createElement( "basket-sync" );
    root.setAttribute("version", QString(VER));


    QDomElement hostname = createElement(doc, "hostname", QHostInfo::localHostName());
    QDomElement ipaddr = createElement(doc, "ip", "1.2.3.4"); //#TODO: fix it to real ip in loop
    QDomElement port = createElement(doc, "port", "35310");
    QDomElement sid = createElement(doc, "id", sync_id);
    QDomElement sdate = createElement(doc, "date", sync_date.toString("yyyy-MM-dd hh-mm"));

    root.appendChild(hostname);
    root.appendChild(ipaddr);
    root.appendChild(port);
    root.appendChild(sid);
    root.appendChild(sdate);

    QByteArray clearBuffer;
    QBuffer clearFile ( &clearBuffer );
    if ( !clearFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return;

    QTextStream clearStream(&clearFile);
    doc.save(clearStream, 4);
    clearFile.close();

    // cookie socket
    QHostAddress addr("224.0.0.1"); //Multicast address
    QTcpSocket *sock = new QTcpSocket();
    sock->connectToHost(addr, 35310, QIODevice::WriteOnly);
    sock->write(clearBuffer);
    sock->close();
}
