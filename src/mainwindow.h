#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//
#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDomElement>
#include <QSystemTrayIcon>
#include "ui_mainwindow.h"
//
class MainWindow : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public slots:
    void editPwd(QTreeWidgetItem *item);
private:
    bool isTreeParent ( QTreeWidgetItem *child, QTreeWidgetItem *parent );
    void setTreeExpanded( QTreeWidgetItem *topitem, bool expand );
    void changeItemParent ( QTreeWidgetItem *source, QTreeWidgetItem *destLocation );
    void closeEvent(QCloseEvent *event);
    void changeCurrentPassword();
    QTreeWidgetItem *getParentForSelectedItem ();
    void allowActions ( bool yes );
    void changePasswordInItem (QTreeWidgetItem *item, QString newPassword);
    void addTreeItemToDom(QDomElement *domRoot, QTreeWidgetItem *item);
    void showPwdItem(QTreeWidgetItem *item, bool isShowPwd);
    void editElement(QTreeWidgetItem *item, QString name, QString login, QString pwd);
    bool parseDocument( QDomDocument doc );
    bool parseElement( QTreeWidgetItem *parent, QDomElement element );
    bool querySave();
    void loadDatabase();
    void saveDatabase();
    void saveAs();
    void newDatabase( bool isInteracrive = false );
    void createTreeWidget();
    void setModif(bool modificator);
    QByteArray hashPassword( QString pwd );
    QTreeWidget *mainTree;
    QString fileName;
    QByteArray mainPassword;
    bool isModified;
    QIcon recordIcon;
    QIcon folderIcon;
    QIcon folderCloseIcon;
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
    void currentItemChanged (  );
    void treeItemDoubleClicked ( QTreeWidgetItem *item );
    void treeItemExpanded ( QTreeWidgetItem *item );
    void treeItemCollapsed ( QTreeWidgetItem *item );

    void statusMessageChanged( QString message );
    void iconActivated( QSystemTrayIcon::ActivationReason reason );
    void trayIconClose();
};
#endif




