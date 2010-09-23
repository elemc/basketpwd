#ifndef BASKETUTILS_H
#define BASKETUTILS_H
//
#include <QObject>
#include <gcrypt.h>
//
class BasketUtils : public QObject
{
Q_OBJECT
public:
	QString decrypt(QString cryptbuf, QString pwd);
	QString crypt(QString buf, QString pwd);
        QByteArray decrypt(QByteArray buf, QString pwd);
	QByteArray crypt(QByteArray buf, QString pwd);
	BasketUtils();

        QString errorMessage() { return errorMsg; };
        bool isError() { return errorState; };
        static QByteArray hashPassword ( QString password );

private:
        gcry_error_t error;
        QString errorMsg;
        bool errorState;

        char *crypt16(char* data, int datalen, char* key, char* iv);
        char *decrypt16(char* data, int datalen, char* key, char* iv);

        int strmagiclen(const char *str);
};
#endif
