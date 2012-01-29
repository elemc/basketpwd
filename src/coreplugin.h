#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QObject>
#include "plugin_share.h"

class CorePlugin : public QObject
{
    Q_OBJECT
public:
    explicit CorePlugin(QObject *parent = 0);
    
    virtual ~CorePlugin();

    virtual bool plugin_load();
    virtual bool plugin_start();
    virtual bool plugin_stop();
    virtual bool plugin_unload();

    bool hasMenuEntry() const;
    virtual PluginMenuEntry menuEntry() const;

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;
    virtual quint8 pluginVersionMajor() const;
    virtual quint8 pluginVersionMinor() const;
    virtual QString pluginVersion() const;

    QWidget *settingsWidget() const;

protected:
    void setVersion(quint8 vMajor, quint8 vMinor);

private:
    quint8 versionMajor;
    quint8 versionMinor;

signals:
    void pluginStateChanged();
    void pluginLoaded();
    void pluginUnloaded();
    void pluginStarted();
    void pluginStoped();

public slots:
    virtual void mainApplicationActionActivated();
};

Q_DECLARE_INTERFACE(CorePlugin, "name.elemc.basketpwd.CorePlugin/1.0")

#endif // COREPLUGIN_H
