#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include <QObject>
#include <QDir>
#include <QApplication>
#include <QList>
#include <QPluginLoader>
#include <QMenu>
#include "plugin_share.h"

// Plugin interfaces
#include "coreplugin.h"


#define PLUGINS_DIR "plugins"

class PluginsLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginsLoader(QObject *parent = 0);
    ~PluginsLoader();

    void loadPlugins();
    void startCorePlugins();
    void stopCorePlugins();
    void unloadPlugins();

    void setMenu( QMenu *menu, PluginMenuEntry target );

private:
    QDir pluginDir;
    QList<CorePlugin *> corePluginList;
    QMenu *FileMenu;
    QMenu *EditMenu;
    QMenu *ViewMenu;
    QMenu *HelpMenu;

    QMenu *getMenu(PluginMenuEntry target) const;

signals:
    void LoadPluginsFailed(QString msg);
    void LoadPluginsWarning(QString msg);
    void PluginLoaded(QString name, QString version, QString description);
    void PluginLoadFailed( QString name, QString version );
    void PluginStarted(QString name, QString version);
    void PluginStartFailed(QString name, QString version);
    void PluginStoped(QString name, QString version);
    void PluginStopFailed(QString name, QString version);

public slots:
    
};

#endif // PLUGINSLOADER_H
