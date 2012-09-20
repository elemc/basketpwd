#include "basketcommon.h"

BasketCommon::BasketCommon(QObject *parent) :
    QObject(parent)
{
}
BasketCommon::~BasketCommon()
{
    this->~QObject();
}

QString BasketCommon::getDefaultDirectory()
{
#ifdef Q_WS_WIN
    QString settingDirName = tr("Корзинка паролей");
#else
    QString settingDirName = ".basketpwd";
#endif

#if BASKETPWD_PORTABLE==1
    QString defPath = QString();
#else
    QString defPath = QString(QDir::toNativeSeparators(QDir::homePath())  + QDir::separator() + settingDirName);
#endif
    
    return defPath;
}
