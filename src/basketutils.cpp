#include <QVariant>
#include "basketutils.h"
#if QT_VERSION >= 0x040300
#include <QCryptographicHash>
#endif

BasketUtils::BasketUtils(  ) 
	: QObject()
{
    errorState = false;
    errorMsg = "";
}
QByteArray BasketUtils::hashPassword( QString password )
{
#if QT_VERSION >= 0x040300
    QByteArray hash = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash::Md5 );
    return hash;
#else
    char *hash_ptr = new char[16];
    char *pwd = new char[password.size()];
    qstrcpy(pwd, password.toUtf8().data());
    hash_ptr = (char*)MD5((unsigned char *)pwd, password.size(), NULL);
    if ( hash_ptr ) {
        QByteArray hash = QByteArray( hash_ptr, 16 );
        delete [] pwd;
        return hash;
    }
    delete [] pwd;

    return QByteArray();
#endif
}
QByteArray BasketUtils::toHex(QByteArray rawdata)
{
#if QT_VERSION >= 0x040300
    return rawdata.toHex();
#else
    // This code coping from Qt sources v4.6.3
    QByteArray hex(rawdata.size() * 2, 0);
    char *hexData = hex.data();
    const uchar *data = (const uchar *)rawdata.data();
    for (int i = 0; i < rawdata.size(); ++i) {
        int j = (data[i] >> 4) & 0xf;
        if (j <= 9)
            hexData[i*2] = (j + '0');
         else
            hexData[i*2] = (j + 'a' - 10);
        j = data[i] & 0xf;
        if (j <= 9)
            hexData[i*2+1] = (j + '0');
         else
            hexData[i*2+1] = (j + 'a' - 10);
    }
    return hex;
#endif
}
QByteArray BasketUtils::fromHex(QByteArray hexEncoded)
{
#if QT_VERSION >= 0x040300
    return QByteArray::fromHex(hexEncoded);
#else
    // This code coping from Qt sources v4.6.3
    QByteArray res((hexEncoded.size() + 1)/ 2, 0);
    uchar *result = (uchar *)res.data() + res.size();

    bool odd_digit = true;
    for (int i = hexEncoded.size() - 1; i >= 0; --i) {
        int ch = hexEncoded.at(i);
        int tmp;
        if (ch >= '0' && ch <= '9')
            tmp = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            tmp = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            tmp = ch - 'A' + 10;
        else
            continue;
        if (odd_digit) {
            --result;
            *result = tmp;
            odd_digit = false;
        } else {
            *result |= tmp << 4;
            odd_digit = true;
        }
    }

    res.remove(0, result - (const uchar *)res.constData());
    return res;
#endif
}

QByteArray BasketUtils::crypt(QByteArray buf, QString pwd) //Пароль уже передается в виже HEX хэша
{
    // Превращаем пароль-хэш в ключ/вектор
    // Разбиваем хэш на 2 части
    if ( pwd.length() != 32 ) {// т.е. если пароль не является HEX кодом хеш пароля
        return NULL;
    }

    QByteArray key  = QByteArray().append(pwd.left(16));//QByteArray::fromHex(pwd.toUtf8());//
    QByteArray iv   = QByteArray().append(pwd.right(16));//QByteArray::fromHex(pwd.toUtf8());//

    QByteArray cipherBuffer;

    //==========================================================================================
    // Служебный блок данных
    // Вычисляем размер последнего блока данных
    int tempBlockSize = buf.size() % 16;
    int lastBlockSize = tempBlockSize == 0 ? 16 : tempBlockSize;
    QByteArray serviceBlock = QByteArray( QVariant(lastBlockSize).toByteArray() );
    while ( serviceBlock.size() < 16 )
        serviceBlock.append('\0');
    char *cipher = crypt16( serviceBlock.data(), serviceBlock.size(), key.data(), iv.data() );

    if (cipher != NULL) {
        QByteArray ciph16 = QByteArray( cipher, 16 );
        cipherBuffer.append(ciph16);
        //free (cipher);
        delete [] cipher;
    }
    else {
        return NULL;
    }
    // Конец служебного блока данных
    //==========================================================================================



    // Создаем цикл перебора буфера данных по 16 байт
    for ( int i = 0; i < buf.size(); i+=16 )
    {
        QByteArray buf16;// = QByteArray::
        if (i + 16 <= buf.size())
            buf16 = buf.mid(i, 16);
        else {
            buf16 = buf.mid(i);
        }

        if ( buf16.size() == 16 )
            ;//buf16.append('\0');
        else {
            while ( buf16.size() < 15 )
                buf16.append('\0');
            buf16.append( buf.size() - i );
        }

        char *cipher = crypt16( buf16.data(), buf16.size(), key.data(), iv.data() );

        if (cipher != NULL) {
            QByteArray ciph16 = QByteArray( cipher, 16 );
            cipherBuffer.append(ciph16);
            delete [] cipher;
        }
    }

    return cipherBuffer;
}
QString BasketUtils::crypt(QString buf, QString pwd)
{
    QByteArray cipherBuf = crypt ( buf.toUtf8(), pwd );
    return toHex(cipherBuf);
}
QByteArray BasketUtils::decrypt(QByteArray buf, QString pwd)
{
    if ( pwd.length() != 32 ) {// т.е. если пароль не является HEX кодом хеш пароля
        return NULL;
    }

    QByteArray key  = QByteArray().append(pwd.left(16));
    QByteArray iv   = QByteArray().append(pwd.right(16));

    QByteArray cipherBuffer;
    int lastBlockSize = 0;
    // Создаем цикл перебора буфера данных по 16 байт
    for ( int i = 0; i < buf.size(); i+=16 )
    {
        QByteArray buf16 = buf.mid(i, 16);
        if ( buf16.size() == 16 )
            ;//buf16.append('\0');
        else {
            buf16.resize(16);
        }

        const char *cipher = decrypt16( buf16.data(), buf16.size(), key.data(), iv.data() );

        if (cipher != NULL) {
            QByteArray ciph16;
            if ( i == 0 ) { // Первый, служебный блок данных
                    ciph16 = QByteArray( cipher, strlen(cipher) );
                    lastBlockSize = ciph16.toInt();
            }
            else if ( i + 16 >= buf.size() ) { // Последний блок данных
                ciph16 = QByteArray( cipher, lastBlockSize );
            }
            else
                ciph16 = QByteArray( cipher, 16 );
            if (i != 0)
                cipherBuffer.append(ciph16);
        }

        delete [] cipher;
    }

    return cipherBuffer;
}
QString BasketUtils::decrypt(QString cryptbuf, QString pwd)
{
    QByteArray buf = decrypt ( fromHex(cryptbuf.toLatin1()), pwd );
    return QString::fromUtf8( buf );
	
}

char *BasketUtils::openssl_crypt(char *data, int datalen, char *key, char *iv, int enc)
{
    char *temp_iv = new char[16];
    bastrcpy(temp_iv, iv, 16);

    AES_KEY aes_key;
    if ( enc == AES_ENCRYPT )
        AES_set_encrypt_key((unsigned char*)key, qstrlen(key)*8, &aes_key);
    else
        AES_set_decrypt_key((unsigned char*)key, qstrlen(key)*8, &aes_key);

    quint8 *out_data = new quint8[datalen];
    if ( !out_data ) {
        errorMsg = trUtf8("Ошибка выделения памяти для выходного буфера!");
        errorState = true;
        return NULL;
    }

    AES_cbc_encrypt((unsigned char*)data, (unsigned char*)out_data, datalen, &aes_key, (unsigned char *)temp_iv, enc);
    if ( !out_data ) {
        errorMsg = trUtf8("Ошибка процесса расшифровки данных!");
        errorState = true;
        return NULL;
    }
    delete [] temp_iv;

    return (char *)out_data;
}
char *BasketUtils::crypt16(char *data, int datalen, char *key, char *iv)
{
    return openssl_crypt(data, datalen, key, iv, AES_ENCRYPT);
}
char *BasketUtils::decrypt16(char *data, int datalen, char *key, char *iv)
{
    return openssl_crypt(data, datalen, key, iv, AES_DECRYPT);
}

int BasketUtils::strmagiclen(const char *str)
{
    int lastCountBytes = str[15];
    if ( lastCountBytes == 15 )
        return 15;
    else if ( lastCountBytes > 15 )
        return 16;
    else {
        return lastCountBytes;
    }
}

void BasketUtils::bastrcpy(char *dest, char *src, int len)
{
    if (!dest)
        return;
    if ( !src ) {
        return;
    }
    for ( int i = 0; i < len; i++ )
        dest[i] = src[i];
}
