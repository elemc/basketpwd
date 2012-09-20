#include "basketcommon.h"

BasketCommon::BasketCommon(QObject *parent) :
    QObject(parent)
{
}
BasketCommon::~BasketCommon()
{
}

QString BasketCommon::getDefaultDirectory()
{
#ifdef Q_WS_WIN
    QString settingDirName = tr("Корзинка паролей");
#else
    QString settingDirName = ".basketpwd";
#endif

#ifdef BASKETPWD_PORTABLE
    QString defPath = QDir::toNativeSeparators(QDir::currentPath());
#else
    QString defPath = QString(QDir::toNativeSeparators(QDir::homePath())  + QDir::separator() + settingDirName);
#endif
    
    return defPath;
}
