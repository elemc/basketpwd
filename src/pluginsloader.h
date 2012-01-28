#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include <QObject>
#include <QDir>
#include <QApplication>
#include <QList>
#include <QPluginLoader>

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

private:
    QDir pluginDir;
    QList<CorePlugin *> corePluginList;

signals:
    void LoadPluginsFailed(QString msg);
    void LoadPluginsWarning(QString msg);
    void PluginLoaded(QString name, QString version, QString description);
    void PluginLoadFailed( QString name, QString version );
    void PluginStarted(QString name, QString version);
    void PluginStartFailed(QString name, QString version);
public slots:
    
};

#endif // PLUGINSLOADER_H
