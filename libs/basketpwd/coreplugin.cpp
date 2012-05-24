#include "coreplugin.h"

CorePlugin::CorePlugin(QObject *parent) :
    QObject(parent)
{
    versionMajor = 0;
    versionMinor = 0;

    root_item = NULL;
    _workplace = NULL;

    init_plugin_information();
}

CorePlugin::~CorePlugin()
{
}

void CorePlugin::init_plugin_information()
{
    plugin_name = QString("");
    plugin_description = QString("");
    setVersion(0,1);
}

bool CorePlugin::plugin_load()
{
    return true;
}
bool CorePlugin::plugin_unload()
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
    return plugin_name;
}
QString CorePlugin::pluginDescription() const
{
    return plugin_description;
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

BasketBaseItem *CorePlugin::rootItem() const {
    return root_item;
}
void CorePlugin::setRootItem(BasketBaseItem *item) {
    root_item = item;
}
void CorePlugin::setWorkPlace(QMainWindow *mw)
{
    _workplace = mw;
}
QMainWindow *CorePlugin::workplace() const {
    return _workplace;
}
