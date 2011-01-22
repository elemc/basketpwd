#include "syncclientthread.h"

SyncClientThread::SyncClientThread(QTcpSocket *client, QObject *parent) :
    QThread(parent)
{
    clientSocket = client;
}
void SyncClientThread::run()
{
    if ( !clientSocket )
        return;

    QTextStream clientData(clientSocket);
    QString cdata = clientData.readAll();

    QDomDocument doc;
    QString errMsg;
    int errLine, errCol;

    bool readResult = doc.setContent( cdata, &errMsg, &errLine, &errCol );
    if ( !readResult ) {
        emit readError(errMsg, errLine, errCol);
        return;
    }

    parseDocument(doc);
    clientSocket->close();
}
void SyncClientThread::parseDocument(QDomDocument &doc)
{
    QString hostname, ip, listid;
    QDateTime listdate;
    quint16 port = 0;

    QDomElement rootElement = doc.documentElement();
    if ( rootElement.tagName() != "basket-sync" ) {
        emit readError(QString("Wrong format of request."), 0, 0);
        return;
    }
    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if ( !n.isElement() )
            continue;
        QDomElement elem = n.toElement();

        if ( elem.tagName() == "hostname" )
            hostname = elem.text();
        else if ( elem.tagName() == "ipaddr" )
            ip = elem.text();
        else if ( elem.tagName() == "port" )
            port = elem.text().toInt();
        else if ( elem.tagName() == "id" )
            listid = elem.text();
        else if ( elem.tagName() == "date" )
            listdate = QDateTime::fromString(elem.text(), "yyyy-MM-dd hh-mm");
    }

    if ( (hostname.size() + ip.size() + listid.size() + port) > 0 )
        emit foundBasketHost(hostname, ip, port, listid, listdate);
}
