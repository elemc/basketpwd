#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QObject>
#include <QMainWindow>
#include "plugin_share.h"
#include "basketbaseitem.h"
#include "basketpwd-libs_global.h"

class BASKETPWDLIBSSHARED_EXPORT CorePlugin : public QObject
{
    Q_OBJECT
public:
    explicit CorePlugin(QObject *parent = 0);
    
    virtual ~CorePlugin();

    virtual bool plugin_load();
    virtual bool plugin_unload();

    bool hasMenuEntry() const;
    virtual PluginMenuEntry menuEntry() const;

    QString pluginName() const;
    QString pluginDescription() const;
    quint8 pluginVersionMajor() const;
    quint8 pluginVersionMinor() const;
    QString pluginVersion() const;

    QWidget *settingsWidget() const;

    void setRootItem(BasketBaseItem *item);
    void setWorkPlace(QMainWindow *mw);

protected:
    virtual void init_plugin_information();
    void setVersion(quint8 vMajor, quint8 vMinor);
    BasketBaseItem *rootItem() const;
    QMainWindow *workplace() const;

    QString plugin_name;
    QString plugin_description;

private:
    quint8 versionMajor;
    quint8 versionMinor;
    BasketBaseItem *root_item;
    QMainWindow *_workplace;

signals:
    void pluginStateChanged();
    void pluginLoaded();
    void pluginUnloaded();

public slots:
    virtual void mainApplicationActionActivated();
};

Q_DECLARE_INTERFACE(CorePlugin, "name.elemc.basketpwd.CorePlugin/1.0")

#endif // COREPLUGIN_H
