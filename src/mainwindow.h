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
#include <QSettings>
#include <QTreeView>
#include <QStyleFactory>

#include "ui_mainwindow.h"
#include "../model/basketmodel.h"
#include "../model/basketbaseitem.h"
#include "firstnetworksender.h"

#define SORTING "SortingEnabled"

//
class MainWindow : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public slots:

private:
    void addItemToModel(bool isFolder);
    void closeEvent(QCloseEvent *event);
    void changeCurrentPassword();
    void allowActions ( bool yes );

    bool querySave();
    void loadDatabase();
    void saveDatabase();
    void saveAs();
    void newDatabase( bool isInteracrive = false );
    void createTreeWidget();
    void initChangeStyles();
    void setModif(bool modificator);
    void changeSortMode();
    void restoreTrayIcon();

    void initVariables();

    QByteArray hashPassword( QString pwd );

    QTreeView *tree;
    BasketModel *model;

    QString fileName;
    QByteArray mainPassword;
    bool    isModified;
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
    QString preDefaultPath;
    bool dontCloseApp;

    // Добавлено 0.4.1
    QString globalStyle;

    // Добавлено 0.4.4
    bool sortingEnabled;
    QActionGroup *primaryActions;

    // добавлено 0.4.5
    FirstNetworkSender *firstNetSender;
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

    // Помощь
    void on_actionHelpAbout_triggered();
    void on_actionHelpAboutQt_triggered();

    //Прочие
    void currentItemChanged ( QModelIndex current_index, QModelIndex previus_index );
    void treeItemDoubleClicked ( QModelIndex index );

    void statusMessageChanged( QString message );
    void iconActivated( QSystemTrayIcon::ActivationReason reason );
    void trayIconClose();

    void onModelDataChanged();
    void slotChangeStypeApp(QAction *styleAct);

    QString getDefaultDirectory() const;

    void changeFoldStatus(const QModelIndex &idx = QModelIndex());
    void on_actionViewPrimaryChecks_triggered(bool checked);
    void generateContextPrimaries();
    void primaryActionsTriggered(QAction *act);

    void NetworkError(QString errmsg);
};
#endif




