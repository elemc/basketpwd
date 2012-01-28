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
void CorePlugin::plugin_load()
{
}
void CorePlugin::plugin_unload()
{
}
void CorePlugin::plugin_start()
{
}
void CorePlugin::plugin_stop()
{
}

bool CorePlugin::menuEntryPresent() const
{
    if (menuEntry() == MenuEntryNone)
        return false;

    return true;
}
CorePlugin::PluginMenuEntry CorePlugin::menuEntry() const
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
