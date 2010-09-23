#include <QVariant>
#include "basketutils.h"
#if QT_VERSION >= 0x040300
#include <QCryptographicHash>
#endif
#include <QDebug>

//
BasketUtils::BasketUtils(  ) 
	: QObject()
{
    errorState = false;
    errorMsg = "";
}
//
QByteArray BasketUtils::hashPassword( QString password )
{
#if QT_VERSION >= 0x040300
    QByteArray hash = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash::Md5 );
    qDebug() << hash.toHex() << hash.size();
    return hash;
#else
    char *hash_ptr = new char[16];
    gcry_md_hash_buffer(GCRY_MD_MD5, hash_ptr, password.toUtf8(), password.size());
    if ( hash_ptr ) {
        QByteArray hash = QByteArray(hash_ptr, 16);
        delete [] hash_ptr;
        return hash;
    }

    return QByteArray();
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
        free (cipher);
    }
    else {
        qDebug ("Ошибка: [%s]", errorMsg.toUtf8().data());
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
            free (cipher);
        }
        else {
            qDebug ("Ошибка: [%s]", errorMsg.toUtf8().data());
        }
    }

    return cipherBuffer;
}
QString BasketUtils::crypt(QString buf, QString pwd)
{
    QByteArray cipherBuf = crypt ( buf.toUtf8(), pwd );
    return cipherBuf.toHex();
}
QByteArray BasketUtils::decrypt(QByteArray buf, QString pwd)
{
    if ( pwd.length() != 32 ) {// т.е. если пароль не является HEX кодом хеш пароля
        return NULL;
    }

    QByteArray key  = QByteArray().append(pwd.left(16));//QByteArray::fromHex(pwd.toUtf8());//
    QByteArray iv   = QByteArray().append(pwd.right(16));//QByteArray::fromHex(pwd.toUtf8());//

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
        else {
            qDebug ("Ошибка: [%s]", errorMsg.toUtf8().data());
        }
    }


    return cipherBuffer;
}
QString BasketUtils::decrypt(QString cryptbuf, QString pwd)
{
    QByteArray buf = decrypt ( QByteArray::fromHex(cryptbuf.toLatin1()), pwd );
    return QString::fromUtf8( buf );
	
}

// Шифрует блок в 16 байт
char *BasketUtils::crypt16(char *data, int datalen, char *key, char *iv)
{
    // Проверка на размер данных/ключа/вектора
    //int datalen = sizeof(data) * sizeof(QChar); //strlen(data);
    if ( datalen != 16 ) {
        errorMsg = trUtf8("Неверный размер буфера данных, требуется передать 16 байт, а передано %1").arg(QVariant(datalen).toString());
        errorState = true;
        return NULL;
    }
    int keylen = strlen(key);
    if ( keylen != 16 ) {
        errorMsg = trUtf8("Неверный размер ключа, требуется передать 16 байт, а передано %1").arg(QVariant(keylen).toString());
        errorState = true;
        return NULL;
    }
    int ivlen = strlen(iv);
    if ( ivlen != 16 ) {
        errorMsg = trUtf8("Неверный размер вектора, требуется передавать 16 байт, а передано %1").arg(QVariant(ivlen).toString());
        errorState = true;
        return NULL;
    }

    // Контекст
    gcry_cipher_hd_t handle;

    // Открываем контекст шифрования
    error = gcry_cipher_open (&handle, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CBC, 0);

    if (error) {
        errorMsg = trUtf8("Ошибка открытия контекста. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    // Устанавливаем ключ
    error = gcry_cipher_setkey(handle, key, keylen);

    if (error) {
        errorMsg = trUtf8("Ошибка установки ключа. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    // Устанавливаем вектор
    error = gcry_cipher_setiv(handle, iv, ivlen);
    if (error) {
        errorMsg = trUtf8("Ошибка установки вектора. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    // Выделяем память для хранения выходного буфера
    char* out = (char*) gcry_calloc(datalen, 1);

    // Собственно шифруем данные
    error = gcry_cipher_encrypt(handle, (unsigned char *)out, datalen, (unsigned char *)data, datalen);

    if (error) {
        errorMsg = trUtf8("Ошибка шифрации данных. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    gcry_cipher_close(handle);
    /* "отпускаем" контекст */

    return out;
}

// Расшифровывает блок в 16 байт
char *BasketUtils::decrypt16(char *data, int datalen, char *key, char *iv)
{
    // Проверка на размер данных/ключа/вектора
    //int datalen = sizeof(data) * sizeof(QChar); //strlen(data);
    if ( datalen != 16 ) {
        errorMsg = trUtf8("Неверный размер буфера данных, требуется передать 16 байт, а передано %1").arg(QVariant(datalen).toString());
        errorState = true;
        return NULL;
    }
    int keylen = strlen(key);
    if ( keylen != 16 ) {
        errorMsg = trUtf8("Неверный размер ключа, требуется передать 16 байт, а передано %1").arg(QVariant(keylen).toString());
        errorState = true;
        return NULL;
    }
    int ivlen = strlen(iv);
    if ( ivlen != 16 ) {
        errorMsg = trUtf8("Неверный размер вектора, требуется передавать 16 байт, а передано %1").arg(QVariant(ivlen).toString());
        errorState = true;
        return NULL;
    }

    // Контекст
    gcry_cipher_hd_t handle;

    // Открываем контекст шифрования
    error = gcry_cipher_open (&handle, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CBC, 0);

    if (error) {
        errorMsg = trUtf8("Ошибка открытия контекста. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    // Устанавливаем ключ
    error = gcry_cipher_setkey(handle, key, keylen);

    if (error) {
        errorMsg = trUtf8("Ошибка установки ключа. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    // Устанавливаем вектор
    error = gcry_cipher_setiv(handle, iv, ivlen);
    if (error) {
        errorMsg = trUtf8("Ошибка установки вектора. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    // Выделяем память для хранения выходного буфера
    char* out = (char*) gcry_calloc(datalen, 1);

    // Собственно шифруем данные
    error = gcry_cipher_decrypt(handle, (unsigned char *)out, datalen, (unsigned char *)data, datalen);

    if (error) {
        errorMsg = trUtf8("Ошибка дешифрации данных. [%1]").arg( QString::fromUtf8( gcry_strerror(error) ));
        errorState = true;
        return NULL;
    }

    gcry_cipher_close(handle);
    /* "отпускаем" контекст */

    return out;
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
