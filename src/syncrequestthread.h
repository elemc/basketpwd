#ifndef SYNCREQUESTTHREAD_H
#define SYNCREQUESTTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QHostInfo>
#include <QDomDocument>
#include <QDateTime>
#include <QBuffer>

class SyncRequestThread : public QThread
{
    Q_OBJECT
public:
    explicit SyncRequestThread(QObject *parent = 0);
    void setSyncInfo(QString id, QDateTime dt);
protected:
    void run();
private:
    QString     sync_id;
    QDateTime   sync_date;

    QDomElement createElement(QDomDocument &doc, QString name, QString value);
signals:

public slots:

};

#endif // SYNCREQUESTTHREAD_H
