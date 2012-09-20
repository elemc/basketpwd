#ifndef BASKETCOMMON_H
#define BASKETCOMMON_H

#include "config.h"
#include <QObject>
#include <QDir>

class BasketCommon : public QObject {
Q_OBJECT

public:
    explicit BasketCommon(QObject *parent = 0);
    ~BasketCommon();
    static QString getDefaultDirectory();
};

#endif
