#include "pluginsloader.h"

PluginsLoader::PluginsLoader(QObject *parent) :
    QObject(parent)
{
    pluginDir.setPath(QApplication::applicationDirPath());

    // NULL to all menu entries
    setMenu(0, MenuEntryFile);
    setMenu(0, MenuEntryEdit);
    setMenu(0, MenuEntryView);
    setMenu(0, MenuEntryHelp);
}
PluginsLoader::~PluginsLoader()
{
    unloadPlugins();
}
void PluginsLoader::loadPlugins()
{
    if ( !pluginDir.cd( PLUGINS_DIR ) ) {
        emit LoadPluginsFailed(tr("Не удалось найти каталог '%1'!").arg(PLUGINS_DIR));
        return;
    }

    foreach( QString pluginFileName, pluginDir.entryList(QDir::Files) ) {
        QPluginLoader loader( pluginDir.absoluteFilePath(pluginFileName) );

        // CorePlugin
        if ( CorePlugin *core_plugin = qobject_cast<CorePlugin *>( loader.instance() ) ) {
            if ( core_plugin->hasMenuEntry() ) {
                QMenu *menuForPlugin = getMenu(core_plugin->menuEntry());
                // TODO: add QAction into menu
                Q_UNUSED(menuForPlugin);
            }
            corePluginList.append(core_plugin);
            if ( core_plugin->plugin_load() )
                emit PluginLoaded( core_plugin->pluginName(), core_plugin->pluginVersion(), core_plugin->pluginDescription() );
            else
                emit PluginLoadFailed( core_plugin->pluginName(), core_plugin->pluginVersion() );

        }
        else {
            emit LoadPluginsWarning( tr("Интерфейс плагина %1 не известен.").arg(pluginFileName) );
        }
    }
}
void PluginsLoader::startCorePlugins()
{
    foreach ( CorePlugin *core_plugin, corePluginList ) {
        if ( !core_plugin )
            continue;
        bool result = core_plugin->plugin_start();
        if ( result )
            emit PluginStarted(core_plugin->pluginName(), core_plugin->pluginVersion());
        else
            emit PluginStartFailed(core_plugin->pluginName(), core_plugin->pluginVersion());
    }
}
void PluginsLoader::stopCorePlugins()
{
    foreach( CorePlugin *core_plugin, corePluginList ) {
        if ( !core_plugin )
            continue;
        bool result = core_plugin->plugin_stop();
        if ( result )
            emit PluginStoped(core_plugin->pluginName(), core_plugin->pluginVersion());
        else
            emit PluginStopFailed(core_plugin->pluginName(), core_plugin->pluginVersion());
    }
}
void PluginsLoader::unloadPlugins()
{
    foreach( CorePlugin *core_plugin, corePluginList ) {
        if ( core_plugin )
            delete core_plugin;
        core_plugin = 0;
    }
    corePluginList.clear();
}

void PluginsLoader::setMenu(QMenu *menu, PluginMenuEntry target)
{
    switch ( target ) {
    case MenuEntryNone:
        break;
    case MenuEntryFile:
        FileMenu = menu;
        break;
    case MenuEntryEdit:
        EditMenu = menu;
        break;
    case MenuEntryView:
        ViewMenu = menu;
    case MenuEntryHelp:
        HelpMenu = menu;
    default:
        break;
    }
}

QMenu *PluginsLoader::getMenu(PluginMenuEntry target) const
{
    QMenu *result = 0;
    switch ( target ) {
    case MenuEntryNone:
        break;
    case MenuEntryFile:
        result = FileMenu;
        break;
    case MenuEntryEdit:
        result = EditMenu;
        break;
    case MenuEntryView:
        result = ViewMenu;
    case MenuEntryHelp:
        result = HelpMenu;
    default:
        break;
    }

        return result;
}
