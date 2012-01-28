#include "pluginsloader.h"

PluginsLoader::PluginsLoader(QObject *parent) :
    QObject(parent)
{
    pluginDir.setPath(QApplication::applicationDirPath());
}
PluginsLoader::~PluginsLoader()
{
    foreach( CorePlugin *core_plugin, corePluginList ) {
        if ( core_plugin )
            delete core_plugin;
        core_plugin = 0;
    }
    corePluginList.clear();
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
        bool result = core_plugin->plugin_start();
        if ( result )
            emit PluginStarted(core_plugin->pluginName(), core_plugin->pluginVersion());
        else
            emit PluginStartFailed(core_plugin->pluginName(), core_plugin->pluginVersion());
    }
}
