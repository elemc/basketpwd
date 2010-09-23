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
    gcry_md_hash_buffer(GCRY_MD_MD5, hash_ptr, password.toUtf8(), password.size());
    if ( hash_ptr ) {
        QByteArray hash = QByteArray(hash_ptr, 16);
        delete [] hash_ptr;
        return hash;
    }

    return QByteArray();
#endif
}
QByteArray BasketUtils::toHex(QByteArray rawdata)
{
#if QT_VERSION >= 0x040300
    return rawdata.toHex();
#else
    // This code coping from Qt sources v4.6.3
    QByteArray hex(rawdata.size() * 2, Qt::Uninitialized);
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
    QByteArray res((hexEncoded.size() + 1)/ 2, Qt::Uninitialized);
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
        free (cipher);
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
            free (cipher);
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
    }


    return cipherBuffer;
}
QString BasketUtils::decrypt(QString cryptbuf, QString pwd)
{
    QByteArray buf = decrypt ( fromHex(cryptbuf.toLatin1()), pwd );
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
