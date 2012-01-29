#include "coreplugin.h"

CorePlugin::CorePlugin(QObject *parent) :
    QObject(parent)
{
    versionMajor = 0;
    versionMinor = 0;
}

CorePlugin::~CorePlugin()
{
}
bool CorePlugin::plugin_load()
{
    return true;
}
bool CorePlugin::plugin_unload()
{
    return true;
}
bool CorePlugin::plugin_start()
{
    return true;
}
bool CorePlugin::plugin_stop()
{
    return true;
}

bool CorePlugin::hasMenuEntry() const
{
    if (menuEntry() == MenuEntryNone)
        return false;

    return true;
}
PluginMenuEntry CorePlugin::menuEntry() const
{
    return MenuEntryNone;
}

QString CorePlugin::pluginName() const
{
    return QString();
}
QString CorePlugin::pluginDescription() const
{
    return QString();
}
QString CorePlugin::pluginVersion() const
{
    return QString("%1.%2").arg(versionMajor).arg(versionMinor);
}
quint8 CorePlugin::pluginVersionMajor() const
{
    return versionMajor;
}
quint8 CorePlugin::pluginVersionMinor() const
{
    return versionMinor;
}

void CorePlugin::setVersion(quint8 vMajor, quint8 vMinor)
{
    versionMajor = vMajor;
    versionMinor = vMinor;
}

QWidget *CorePlugin::settingsWidget() const
{
    return NULL;
}

void CorePlugin::mainApplicationActionActivated()
{
}
