#ifndef EDITFOLDERDIALOG_H
#define EDITFOLDERDIALOG_H

#include "ui_editfolderdialog.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>

class editFolderDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(editFolderDialog)
public:
    explicit editFolderDialog(QWidget *parent = 0, QTreeWidgetItem *editedFolder = 0);
    void setMainTree ( QTreeWidget *mainTree, QTreeWidgetItem *selectedFolder );
    void setFolderName ( QString value );
    QTreeWidgetItem *getParent();
    QString getFolderName();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::editFolderDialog m_ui;
    void parseTreeItem ( QTreeWidgetItem *parent, QTreeWidgetItem *subitem );

    // таблица отношений истинной папки и местной
    QMap<QTreeWidgetItem *, QTreeWidgetItem *> hash;
    // корневой узел дерева
    QTreeWidgetItem *root;
    QTreeWidgetItem *hideEditedFolder;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void currentItemChanged();
};

#endif // EDITFOLDERDIALOG_H
