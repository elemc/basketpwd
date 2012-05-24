#ifndef BASKETUTILS_H
#define BASKETUTILS_H
//
#include <QObject>
#include <openssl/aes.h>
#if QT_VERSION < 0x040300
#include <openssl/md5.h>
#endif
//
#include "basketpwd-libs_global.h"

class BASKETPWDLIBSSHARED_EXPORT BasketUtils : public QObject
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
        static QByteArray toHex(QByteArray data);
        static QByteArray fromHex(QByteArray hex);

private:
        //gcry_error_t error;
        QString errorMsg;
        bool errorState;

        char *crypt16(char* data, int datalen, char* key, char* iv);
        char *decrypt16(char* data, int datalen, char* key, char* iv);
        char *openssl_crypt(char *data, int datalen, char *key, char *iv, int enc);

        int strmagiclen(const char *str);
        void bastrcpy(char *dest, char *src, int len);
};
#endif
