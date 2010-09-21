#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QDateTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextStream>
#include <QFile>
#include <QBuffer>
#include <QFileInfo>
#include <QDataStream>
#include <QFileDialog>
#include <QClipboard>
#include <QCloseEvent>
#include <QCryptographicHash>
#include <QSettings>
#include <QTreeView>

#include "ui_mainwindow.h"
#include "../model/basketmodel.h"
#include "../model/basketbaseitem.h"

#define DATE_TIME_FORMAT QString("yyyy-MM-dd/hh:mm")

//
class MainWindow : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public slots:

private:
    /*bool isTreeParent ( QTreeWidgetItem *child, QTreeWidgetItem *parent );
    void changeItemParent ( QTreeWidgetItem *source, QTreeWidgetItem *destLocation );
    QTreeWidgetItem *getParentForSelectedItem ();
    void changePasswordInItem (QTreeWidgetItem *item, QString newPassword);
    void addTreeItemToDom(QDomElement *domRoot, QTreeWidgetItem *item);
    void showPwdItem(QTreeWidgetItem *item, bool isShowPwd);
    void editElement(QTreeWidgetItem *item, QString name, QString login, QString pwd);*/

    void addItemToModel(bool isFolder);
    void closeEvent(QCloseEvent *event);
    void changeCurrentPassword();
    void allowActions ( bool yes );

    void setTreeExpanded( bool expand, QModelIndex topitem = QModelIndex() );
    bool querySave();
    void loadDatabase();
    void saveDatabase();
    void saveAs();
    void newDatabase( bool isInteracrive = false );
    void createTreeWidget();
    void setModif(bool modificator);
    QByteArray hashPassword( QString pwd );
    //QTreeWidget *mainTree;

    QTreeView *tree;
    BasketModel *model;
    //ChangePasswordDelegate *delegate;

    QString fileName;
    QByteArray mainPassword;
    bool isModified;
    bool    isSimpleXML;

    // Добавлено 0.2.5
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    void createTrayIcon();
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    void triggeredTrayIcon();
    unsigned short lastTrayState; // Переменная, в которой хранится предыдущее состояние для щелчка
                                  // 0 - скрыто, 1 - нормально, 2 - развернуто

    // Добавлено 0.2.6
    QString defaultPath;
    bool dontCloseApp;

    // Добавлено 0.3.7
    QString databaseIdentifier;
    QDateTime lastModified;

public:
    MainWindow( QWidget * parent = 0, Qt::WFlags f = 0 );
    ~MainWindow();

private slots:
    // Файл
    void on_actionCopyLogin_triggered();
    void on_actionSettings_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionChangeCurrentPassword_triggered();
    void on_actionExit_triggered();

    // Правка
    void on_actionEditAddPwd_triggered();
    void on_actionEditAddFolder_triggered();
    void on_actionEditEdit_triggered();
    void on_actionEditDel_triggered();
    void on_actionShowPwd_triggered(bool checked);
    void on_actionCopyToClipboard_triggered();

    // Вид
    void on_actionViewExpand_triggered();
    void on_actionViewUnExpand_triggered();

    // Помощь
    void on_actionHelpAbout_triggered();
    void on_actionHelpAboutQt_triggered();

    //Прочие
    void currentItemChanged ( QModelIndex current_index, QModelIndex previus_index );
    void treeItemDoubleClicked ( QModelIndex index );
//    void treeItemExpanded ( QTreeWidgetItem *item );
//    void treeItemCollapsed ( QTreeWidgetItem *item );

    void statusMessageChanged( QString message );
    void iconActivated( QSystemTrayIcon::ActivationReason reason );
    void trayIconClose();
};
#endif




