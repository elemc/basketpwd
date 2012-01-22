#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QObject>

class CorePlugin : public QObject
{
    Q_OBJECT
public:
    explicit CorePlugin(QObject *parent = 0);
    
    virtual ~CorePlugin() {} ;

    virtual void plugin_load() {} ;
    virtual void plugin_start() {} ;
    virtual void plugin_unload() {} ;

signals:
    void pluginStateChanged();
public slots:
    
};

Q_DECLARE_INTERFACE(CorePlugin, "name.elemc.basketpwd.CorePlugin/1.0")

#endif // COREPLUGIN_H
