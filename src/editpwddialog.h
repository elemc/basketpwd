#ifndef EDITPWDDIALOG_H
#define EDITPWDDIALOG_H
//
#include <QDialog>
#include "ui_editPwd.h"
//
class editPwdDialog : public QDialog, public Ui::editPwd
{
Q_OBJECT
private:
    QString elementPassword;
    QString elementLogin;
    QString elementName;
    void parseTreeItem ( QTreeWidgetItem *parent, QTreeWidgetItem *subitem );

    // таблица отношений истинной папки и местной
    QMap<QTreeWidgetItem *, QTreeWidgetItem *> hash;
    // корневой узел дерева
    QTreeWidgetItem *root;

public:
    void setElement(QString eName, QString eLogin, QString ePwd);
    QString getElementPassword() { return elementPassword; }
    QString getElementLogin() { return elementLogin; }
    QString getElementName() { return elementName; }
    editPwdDialog( QWidget * parent = 0, Qt::WFlags f = 0 );
    QTreeWidgetItem *getParent();
    void setMainTree ( QTreeWidget *mainTree, QTreeWidgetItem *selectedFolder );

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void currentItemChanged();
};
#endif





