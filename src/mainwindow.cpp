#include "mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include "editpwddialog.h"
#include <QDomDocument>
#include <QDomText>
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
#include "basketutils.h"
#include "changepassword.h"
#include "editfolderdialog.h"
#include "../aboutdialog.h"
#include "settingsdialog.h"

// Конструктор/деструктор
MainWindow::MainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
    setupUi(this);

    //mainTree = new QTreeWidget( );

    model = new BasketModel(this);
    tree = new QTreeView( this );
    tree->setModel( model );
    setCentralWidget(tree);

    connect ( tree->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentItemChanged(QModelIndex,QModelIndex)));

    newDatabase();

    //Пытаемся обнаружить и загрузить файл с паролями по-умолчанию
    // Добавлено 0.2.6
    QSettings set;
    defaultPath = set.value(tr("PathToDef"), QString(QDir::currentPath())).toString();
    dontCloseApp = set.value(tr("DontCloseApp"), false).toBool();

    QFileInfo defaultFile ( defaultPath + QDir::separator() + "default.cxml" );
    if ( defaultFile.exists() ) {
        fileName = defaultFile.absoluteFilePath();
        loadDatabase();
    }
    else {
        newDatabase( true );
        fileName = "default.cxml";
        setModif( false );
    }
    actionCopyToClipboard->setEnabled( false );
    actionCopyLogin->setEnabled( false );

    connect(statusbar, SIGNAL(messageChanged(QString)), this, SLOT(statusMessageChanged(QString)) );

    // Добавление версии 0.2.5 организация трея
    createTrayIcon();
}
MainWindow::~MainWindow() {
    if ( quitAction )
        delete quitAction;

    if ( restoreAction )
        delete restoreAction;

    if ( maximizeAction )
        delete maximizeAction;

    if ( minimizeAction )
        delete minimizeAction;

    if ( trayIconMenu )
        delete trayIconMenu;

    if ( trayIcon )
        delete trayIcon;
}

// Виртуалы
void MainWindow::closeEvent(QCloseEvent *event)
{
    if ( dontCloseApp ) {
        triggeredTrayIcon();
        event->ignore();
        return;
    }

    if ( querySave() )
        event->accept();
    else
        event->ignore();
}

// Инициализаторы
void MainWindow::createTrayIcon()
{
    minimizeAction = new QAction(trUtf8("&Свернуть"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(trUtf8("Развернуть на весь &экран"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(trUtf8("Восстановить &прежний размер"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(trUtf8("&Закрыть"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(trayIconClose())); //SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
            SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->setIcon(QIcon(":/images/ProgramIcon"));
    trayIcon->show();
}
void MainWindow::createTreeWidget()
{
        // TODO
//        mainTree->setColumnCount(3); //ну типа для Наименование, Логин и скрытый по-умолчанию пароль
//        mainTree->setSortingEnabled( true );
//        mainTree->setHeaderLabels( QStringList() << trUtf8("Наименование") << trUtf8("Имя (логин)") << trUtf8("Пароль") );

        //Определяем первичный размер колонок
        int widthTree = tree->width();
        tree->setColumnWidth(0, widthTree / 100 * 50);
        tree->setColumnWidth(1, widthTree / 100 * 25);
        tree->setColumnWidth(2, widthTree / 100 * 25);

        // Переопределяем контекстное меню дерева
        QAction *edit_sep = new QAction( this );
        edit_sep->setSeparator( true );
        tree->setContextMenuPolicy( Qt::ActionsContextMenu );
        tree->addAction( actionCopyLogin );
        tree->addAction( actionCopyToClipboard );
        tree->addAction( edit_sep );
        tree->addAction( actionEditAddFolder );
        tree->addAction( actionEditAddPwd );
        tree->addAction( actionEditEdit );
        tree->addAction( actionEditDel );

        //connect (tree, SIGNAL())
        /*connect (mainTree,
                 SIGNAL (itemActivated(QTreeWidgetItem*,int)),
                 this,
                 SLOT (treeItemDoubleClicked (QTreeWidgetItem *)) );

        // Подключаем сигналы сворачивания/разворачивания
        connect ( mainTree,
                  SIGNAL (itemExpanded (QTreeWidgetItem *)),
                          this,
                          SLOT (treeItemExpanded ( QTreeWidgetItem *)) );
        connect ( mainTree,
                  SIGNAL (itemCollapsed (QTreeWidgetItem *)),
                          this,
                          SLOT (treeItemCollapsed ( QTreeWidgetItem *)) );*/
        // TODO: Сделать слоты реакций
        //mainTree->sortByColumn(0, Qt::AscendingOrder );
        // TODO: Сделать сортировку
}

// Собственные процедуры
// Работа с файлом
void MainWindow::newDatabase( bool isInteracrive )
{
        fileName = "";
        setModif(false);
        //mainTree->clear();
        // TODO: Сделать очистку модели
        createTreeWidget();
        mainPassword = QByteArray();

        if ( isInteracrive )
            changeCurrentPassword();
        else
            allowActions ( false );
}
void MainWindow::saveAs()
{
    QFileDialog saveDlg;
    saveDlg.setAcceptMode( QFileDialog::AcceptSave );
    saveDlg.setDefaultSuffix( "cxml" );
    saveDlg.setNameFilters( QStringList() << trUtf8("Шифрованные XML файлы (%1)").arg( "*.cxml" )
    //                        << trUtf8("Файлы XML для переноса между версиями (%1)").arg("*.xml")
                            << trUtf8("Любые файлы (%1)").arg("*") );
    if ( saveDlg.exec() ) {
        if ( saveDlg.selectedFiles().count() > 0 )
            fileName = saveDlg.selectedFiles()[0];
        saveDatabase();
        setModif ( false );
    }
}
void MainWindow::saveDatabase()
{
    QString ext = fileName.right(4);

    QDomDocument    doc;
    QDomElement	root = doc.createElement( trUtf8("basket-passwords") );
    root.setAttribute(QString("ident"), databaseIdentifier);

    // Сохраняем текущий параметр даты/времени
    lastModified = QDateTime::currentDateTime();
    root.setAttribute(QString("modified"), lastModified.toString(DATE_TIME_FORMAT));

    doc.appendChild( root );

    /*for ( int i = 0; i < mainTree->topLevelItemCount(); i++ )
            addTreeItemToDom( &root, mainTree->topLevelItem(i) );*/
    // TODO: Сделать сохранение списка

    QByteArray bufferArray;
    QBuffer outFile ( &bufferArray );
    bool result = outFile.open( QIODevice::WriteOnly | QIODevice::Text );
    if ( !result ) {
        QMessageBox::critical( this, trUtf8("Ошибка доступа к памяти"),
                               trUtf8("Невозможно выделить память для хранения дерева.").arg( fileName ) );
        return;
    }
    QTextStream out(&outFile);
    doc.save(out, 4);
    outFile.close();

    //Пишем в истинный файл
    BasketUtils butil;
    QFile filew ( fileName );
    result = filew.open( QIODevice::WriteOnly );
    if ( !result ) {
        QMessageBox::critical( this, trUtf8("Ошибка доступа к файлу"),
                               trUtf8("Ошибка открытия файла %1 для записи").arg( fileName ) );
        return;
    }

    QByteArray ciphBuf = butil.crypt( bufferArray, mainPassword.toHex() );
    filew.write( ciphBuf );
    filew.close();

    setModif ( false );
}
/*bool MainWindow::parseElement( QTreeWidgetItem *parent, QDomElement element )
{
    //Смотрим, что это папка или запись
    if ( element.tagName() == "folder" ) {
        QString folderName = element.attribute( "name", trUtf8("Без имени") );
        QTreeWidgetItem *item = new QTreeWidgetItem( QStringList() << folderName );
        item->setData(0, Qt::UserRole, 1);
        item->setData(0, Qt::DecorationRole, folderCloseIcon );

        // добавляем папочку в дерево
        if ( parent == NULL )
            mainTree->addTopLevelItem( item );
        else
            parent->addChild( item );

        // парсируем, все, что внутри
        for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
        {
            if ( n.isElement() )
                parseElement ( item, n.toElement() );
        }

    }
    else if ( element.tagName() == "item" ) {
        QString name = element.attribute( "name", trUtf8("Без имени") );
        QString login, cipherPwd;

        // определяем логин
        QDomNodeList listLogin = element.elementsByTagName("login");
        QDomNodeList listPwd = element.elementsByTagName("pwd");
        if ( listLogin.count() > 0 )
            login = listLogin.at(0).toElement().text();
        if ( listPwd.count() > 0 )
            cipherPwd = listPwd.at(0).toElement().text();

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, name);
        item->setData(0, Qt::UserRole, 0);	//0-элемент, 1 - группа
        item->setData(0, Qt::DecorationRole, recordIcon );
        item->setText(1, login);

        item->setText( 2, trUtf8("* * *") );

        if (!isSimpleXML)
            item->setData( 2, Qt::UserRole, cipherPwd );
        else {
            BasketUtils butil;
            QString arr = butil.crypt(cipherPwd, mainPassword.toHex());
            item->setData( 2, Qt::UserRole, arr );
        }

        // добавляем запись в дерево
        if ( parent == NULL )
            mainTree->addTopLevelItem( item );
        else
            parent->addChild( item );
    }

    return true;
}
bool MainWindow::parseDocument( QDomDocument doc )
{
    QDomElement rootElement = doc.documentElement();
    if ( rootElement.tagName() != "basket-passwords" )
        return false;

    databaseIdentifier  = rootElement.attribute(QString("ident"), QString("default"));
    lastModified        = QDateTime::fromString(rootElement.attribute(QString("modified"), QDateTime::currentDateTime().toString(DATE_TIME_FORMAT)), DATE_TIME_FORMAT);

    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if ( n.isElement() )
            parseElement ( NULL, n.toElement() );
    }

    return true;
}*/
void MainWindow::loadDatabase()
{
    // Обнуляем кнопочку показа паролей, иначе уязвимость
    actionShowPwd->setChecked( false );

    QString ext = fileName.right(4);
    if (ext.toLower() == ".xml")
        isSimpleXML = true;
    else
        isSimpleXML = false;

    QString tempPassword;
    // Запрашиваем и записываем новый пароль для доступа к файлу
    if (!isSimpleXML) {
        bool acceptDlg = false;
        tempPassword = QInputDialog::getText( this, trUtf8("Запрос пароля"),
                            trUtf8("Введите пароль для доступа к файлу %1:").arg( fileName ),
                            QLineEdit::Password, QString(), &acceptDlg );
        if ( (!acceptDlg) || tempPassword.isEmpty() ) {
            newDatabase ( true );
            return;
        }
    }
    else {
        tempPassword = "1234";
        mainPassword = hashPassword(tempPassword);
    }

    // Пытаемся прочитать файл
    QFile inFile ( fileName );
    bool result = inFile.open( QIODevice::ReadOnly );

    if ( !result ) {
        QMessageBox::critical (this, trUtf8("Ошибка чтения файла"),
                               trUtf8( "Файл %1 не может быть прочитан, возможно Вы не обладаете правами на чтение данного файла.").
                               arg(fileName) );
        fileName = "";
        return;
    }
    QByteArray cipherData = inFile.readAll();

    bool fill_result = model->setModelData(cipherData, hashPassword(tempPassword).toHex(), !isSimpleXML);
    // Если же все-таки файл может быть прочитан
    if ( fill_result ) {
        mainPassword = hashPassword(tempPassword);
        databaseIdentifier = model->identifier();
        lastModified = model->lastModified();
    }
    else
        QMessageBox::critical(this, tr("Ошибка чтения файла"), tr("Файл не является файлом XML или пароль не верен!"));
    setModif ( false );
    allowActions( true );
}
bool MainWindow::querySave()
{
    if ( !isModified )
        return true;
    else
    {
        int result = QMessageBox::question(this, trUtf8("Сохранение базы данных"),
                                                trUtf8("Вы не сохранили изменения сделанные в базе данных."),
                                                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                                QMessageBox::Save);
        if ( result == QMessageBox::Cancel )
                return false;
        else if ( result == QMessageBox::Save )
                on_actionSave_triggered();
    }

    return true;
}

// Работа с деревом
void MainWindow::addTreeItemToDom(QDomElement *domRoot, QTreeWidgetItem *item)
{
        QDomDocument doc = domRoot->ownerDocument();
        if ( item == NULL )
                return;

        if ( item->data(0, Qt::UserRole).toInt() == 0 ) {

                QDomElement elem = doc.createElement("item");
                elem.setAttribute(trUtf8("name"), item->data(0, Qt::DisplayRole).toString() );
                QDomElement login = doc.createElement("login");
                QDomElement pwd = doc.createElement("pwd");

                QDomText t_login = doc.createTextNode( item->data(1, Qt::DisplayRole).toString() );

                QDomText t_pwd;
                t_pwd = doc.createTextNode( item->data(2, Qt::UserRole).toString() );

                domRoot->appendChild(elem);
                elem.appendChild(login);
                elem.appendChild(pwd);
                login.appendChild(t_login);
                pwd.appendChild(t_pwd);
        }
        else {
                QDomElement folder = doc.createElement("folder");
                folder.setAttribute(trUtf8("name"), item->data(0, Qt::DisplayRole).toString());
                domRoot->appendChild(folder);

                for ( int i = 0; i < item->childCount(); i++ )
                        addTreeItemToDom ( &folder, item->child(i) );
        }
}
QTreeWidgetItem *MainWindow::getParentForSelectedItem ()
{
    /*if ( mainTree->selectedItems().count() == 0 )
        return NULL;
    else {
        QTreeWidgetItem *folder = mainTree->selectedItems()[0];

        // Проверка на принадлежность к верхней папке
        for ( int i = 0; i < mainTree->topLevelItemCount(); i++ ) {
            if ( mainTree->topLevelItem(i) == folder )
                return folder;
        }

        if ( folder->parent() )
            return folder->parent();
        else
            return NULL;
    }
    return NULL;*/
    // FIXME: Не помню зачем я это делалs
    return 0;
}
void MainWindow::changeItemParent ( QTreeWidgetItem *source, QTreeWidgetItem *destLocation )
{
    // если произошла смена родителя-владельца
    /*if ( source->parent() != destLocation ) {
        QTreeWidgetItem *newitem = source->clone();

        if ( !destLocation )  // если родителя нет (в корень)
            mainTree->addTopLevelItem( newitem );
        else {
            destLocation->addChild( newitem );
            destLocation->setExpanded( true );
        }

        source->~QTreeWidgetItem();
        setModif( true );
        mainTree->setCurrentItem( newitem );
    }
    else
        mainTree->setCurrentItem( source );*/
    // TODO: Сделать перенос записи между группами
}
void MainWindow::editPwd(QModelIndex index)
{
    /*if ( item == NULL ) {
        if ( mainTree->selectedItems().count() == 0 )
            return;
        else
            item = mainTree->selectedItems()[0];
    }*/
    if ( !index.isValid() )
        return;

    /*if ( item->data(0, Qt::UserRole).toInt() == 0) {
        BasketUtils butil;
        editPwdDialog dlg( this );
        dlg.setElement( item->text(0),
                        item->text(1),
                        butil.decrypt(item->data(2, Qt::UserRole).toString(), mainPassword.toHex()) );
        dlg.setMainTree( mainTree, getParentForSelectedItem() );
        if ( dlg.exec() == QDialog::Accepted ) {
            editElement ( item, dlg.getElementName(), dlg.getElementLogin(), dlg.getElementPassword() );
            changeItemParent ( item, dlg.getParent() );
            setModif ( true );
        }
    }
    else {
        editFolderDialog dlg( this, item );
        if ( mainTree->selectedItems().count() == 0 )
            dlg.setMainTree( mainTree, NULL );
        else
            dlg.setMainTree( mainTree, mainTree->selectedItems()[0] );
        dlg.setMainTree( mainTree, getParentForSelectedItem() );
        dlg.setFolderName( item->data(0, Qt::DisplayRole ).toString() );
        if ( !dlg.exec() )
            return; // выходим, если отмена

        item->setData( 0, Qt::DisplayRole, dlg.getFolderName() );
        changeItemParent ( item, dlg.getParent() );
        setModif ( true );
    }*/
    // TODO: Сделать редактирование записи и группы

}
void MainWindow::editElement(QTreeWidgetItem *item, QString name, QString login, QString pwd)
{
        BasketUtils butil;
        item->setText(0, name);
        item->setData(0, Qt::UserRole, 0);	//0-элемент, 1 - группа
        //item->setData(0, Qt::DecorationRole, recordIcon );
        item->setText(1, login);

        if ( actionShowPwd->isChecked() ) {
                item->setText(2, pwd);
                item->setData( 2, Qt::UserRole, butil.crypt(pwd, mainPassword.toHex()) );
        }
        else {
                item->setText( 2, trUtf8("* * *") );
                item->setData( 2, Qt::UserRole, butil.crypt(pwd, mainPassword.toHex()) );
        }
}
void MainWindow::showPwdItem(QTreeWidgetItem *item, bool isShowPwd)
{
        BasketUtils butil;
        if ( item->data(0, Qt::UserRole).toInt() == 0 ) {
                if ( isShowPwd )
                        item->setData( 2, Qt::DisplayRole, butil.decrypt(item->data(2, Qt::UserRole).toString(), mainPassword.toHex()) );
                else
                        item->setData(2, Qt::DisplayRole, trUtf8("* * *"));
        }
        else {
                for ( int i = 0; i < item->childCount(); i++ )
                        showPwdItem ( item->child(i), isShowPwd );
        }
}
void MainWindow::changePasswordInItem (QTreeWidgetItem *item, QString newPassword)
{
        BasketUtils butil;
        if ( item->data(0, Qt::UserRole).toInt() == 0 ) {
            QString cleanPwd = butil.decrypt(item->data( 2, Qt::UserRole ).toString(), mainPassword.toHex());
            item->setData( 2, Qt::UserRole, butil.crypt(cleanPwd, newPassword) );
        }
        else {
                for ( int i = 0; i < item->childCount(); i++ )
                        changePasswordInItem ( item->child(i), newPassword );
        }
}
void MainWindow::setTreeExpanded(bool expand, QModelIndex topitem)
{
    if ( !topitem.isValid() ) {// передан NULL
        for (int i = 0; i < model->rowCount(); i++ )
            setTreeExpanded ( expand, model->index(i, 0) );
    }
    else {
        tree->setExpanded(topitem, expand);
        for (int j = 0; j < model->rowCount(topitem); j++ )
            setTreeExpanded ( expand, model->index(j, 0, topitem) );
    }
}
void MainWindow::treeItemDoubleClicked (QModelIndex index)
{
    if ( !index.isValid() )
        return;

    /*if ( item->data( 0, Qt::UserRole ).toInt() == 0 )
        editPwd ( item );*/
    // TODO: Сделать активацию, открытие диалога редактирования

}
void MainWindow::treeItemExpanded ( QTreeWidgetItem *item )
{
    if (!item)
        return;

    // TODO: Сделать реакцию на развертывание
    //if ( item->data(0, Qt::UserRole).toInt() == 1 );
        //item->setData( 0, Qt::DecorationRole, folderIcon );
}
void MainWindow::treeItemCollapsed ( QTreeWidgetItem *item )
{
    if (!item)
        return;

    // TODO: Сделать реакцию на сворачивание
    //if ( item->data(0, Qt::UserRole).toInt() == 1 )
        //item->setData( 0, Qt::DecorationRole, folderCloseIcon );

    // Проверка на выделенную запись
    /*if ( isTreeParent ( mainTree->currentItem(), item ) )
        mainTree->setCurrentItem( item );*/
}
bool MainWindow::isTreeParent( QTreeWidgetItem *child, QTreeWidgetItem *parent )
{
    if ( !child->parent() )
        return false;

    QTreeWidgetItem *tempParent = child->parent();
    while ( tempParent ) {
        if ( tempParent == parent )
            return true;
        else
            tempParent = tempParent->parent();
    }

    return false;
}

// Остальное
void MainWindow::setModif(bool modificator)
{
    isModified = modificator;
    QCoreApplication *app = QCoreApplication::instance();
    QString winTitle = trUtf8("%1 v%2").arg(app->applicationName()).arg(app->applicationVersion());
    QString strModif = (modificator ? tr("*") : tr(""));
    QString modifFileName = trUtf8(" (без имени) ");
    if ( !fileName.isEmpty() )
        modifFileName = fileName;
    QString id = QString("%1 - %2 (%3) %4").arg(winTitle).arg(databaseIdentifier).arg(lastModified.toString(DATE_TIME_FORMAT)).arg(strModif);
    setWindowTitle(id.trimmed());
    actionSave->setEnabled( modificator );
}
void MainWindow::changeCurrentPassword()
{
    /*ChangePassword cpDlg( this );
    if ( mainPassword.isNull() )
        cpDlg.setNewPassword( true );
    if ( cpDlg.exec() ) {
        if ((( cpDlg.getCurrentPassword().isEmpty() ) && ( mainPassword.isNull() )) || ( hashPassword(cpDlg.getCurrentPassword()) == mainPassword ) ) {
            for ( int i = 0; i < mainTree->topLevelItemCount(); i++ )
                changePasswordInItem ( mainTree->topLevelItem(i), hashPassword(cpDlg.getNewPassword()).toHex() );

            mainPassword = hashPassword(cpDlg.getNewPassword());
            setModif( true );
            allowActions( true );
        }
    }
    else if ( mainPassword.isNull() )
        allowActions ( false );*/
    // TODO: Сделать изменение пароля на список
}
void MainWindow::currentItemChanged ( QModelIndex current_index, QModelIndex previus_index )
{
    Q_UNUSED(previus_index)
    if ( !current_index.isValid() ) {
        actionCopyToClipboard->setEnabled( false );
        actionCopyLogin->setEnabled( false );
        return;
    }
    BasketBaseItem *item = model->itemAtIndex(current_index);
    bool isAct = true;
    if (item->isFolder())
        isAct = false;

    actionCopyToClipboard->setEnabled( isAct );
    actionCopyLogin->setEnabled( isAct );
}
void MainWindow::allowActions( bool yes )
{
    //actionSave->setEnabled( yes );
    actionSaveAs->setEnabled( yes );
    actionEditAddPwd->setEnabled( yes );
    actionEditAddFolder->setEnabled( yes );
    actionEditEdit->setEnabled( yes );
    actionEditDel->setEnabled( yes );
    actionShowPwd->setEnabled( yes );

    // Устанавливаем текст в статусной строке
    if (!yes)
        statusbar->showMessage(
                trUtf8("Вы не задали пароль на новый список данных. Сделать это можно в меню \"Файл\"->\"Сменить текущий пароль\"."));
    else
        statusbar->showMessage(trUtf8("Готово."));
}
QByteArray MainWindow::hashPassword( QString pwd )
{
    QByteArray hash = QCryptographicHash::hash( pwd.toUtf8(), QCryptographicHash::Md5 );
    return hash;
}
void MainWindow::statusMessageChanged( QString message )
{
    if ( !message.isEmpty() )
        return;
    else {
        if ( !actionEditAddPwd->isEnabled() && !actionEditAddFolder->isEnabled() )
            statusbar->showMessage(
                    trUtf8("Вы не задали пароль на новый список данных. Сделать это можно в меню \"Файл\"->\"Сменить текущий пароль\"."));
        else
            statusbar->showMessage(trUtf8("Готово."));
    }
}
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
     switch (reason) {
     case QSystemTrayIcon::Trigger:
         triggeredTrayIcon();
         break;
     case QSystemTrayIcon::DoubleClick:
         triggeredTrayIcon();
         break;
     case QSystemTrayIcon::MiddleClick:
         triggeredTrayIcon();
         break;
     default:
         ;
     }
}
void MainWindow::triggeredTrayIcon()
{
    if ( this->isHidden() )
        show();
    else
        hide();
}

// Слоты
void MainWindow::on_actionExit_triggered()
{
        close();
}
void MainWindow::on_actionNew_triggered()
{
    if ( querySave() )
        newDatabase( true );
}
void MainWindow::on_actionEditAddFolder_triggered()
{
    /*editFolderDialog dlg( this );

    dlg.setMainTree( mainTree, getParentForSelectedItem() );
    if ( !dlg.exec() )
        return;

    QTreeWidgetItem *item = new QTreeWidgetItem( QStringList() << dlg.getFolderName() );
    item->setData(0, Qt::UserRole, 1);
    //item->setData(0, Qt::DecorationRole, folderIcon );

    QTreeWidgetItem *folder = dlg.getParent();
    if ( folder ) {
        folder->addChild( item );
        folder->setExpanded( true );
    }
    else
        mainTree->addTopLevelItem( item );

    mainTree->setCurrentItem( item );
    setModif ( true );*/

    // TODO: Сделать добавление новой папки

}
void MainWindow::on_actionEditAddPwd_triggered()
{
    /*BasketUtils butil;
    editPwdDialog dlg( this );
    dlg.setMainTree( mainTree, getParentForSelectedItem() );
    if ( dlg.exec() == QDialog::Accepted ) {
        QTreeWidgetItem *item = new QTreeWidgetItem( );
        editElement ( item, dlg.getElementName(), dlg.getElementLogin(), dlg.getElementPassword() );

        QTreeWidgetItem *folder = dlg.getParent();
        if ( folder == NULL )
            mainTree->addTopLevelItem( item );
        else {
            folder->addChild( item );
            folder->setExpanded(true);
        }
        mainTree->setCurrentItem( item );
        setModif ( true );
    }*/

    // TODO: Сделать добавление новой записи
}
void MainWindow::on_actionShowPwd_triggered(bool checked)
{
        /*bool acceptDlg = false;
        QString tempPassword;

        if ( checked )
                tempPassword = QInputDialog::getText( this, trUtf8("Запрос пароля"),
                        trUtf8("<b><p>Внимание! Потенциально опасно отображать пароли!</p>\n<p>Если Вы находитесь перед компьютером не один, нажмите кнопку \"Отмена\"</p></b>\nПодтвердите свой пароль:"),
                        QLineEdit::Password, QString(), &acceptDlg );
        else {
                //tempPassword = mainPassword;
                acceptDlg = true;
        }


        if ( (acceptDlg) ) {
            if ( (( checked ) && (hashPassword(tempPassword) == mainPassword) ) || ( !checked ) )
                for ( int i = 0; i < mainTree->topLevelItemCount(); i++ )
                        showPwdItem ( mainTree->topLevelItem(i), checked );
            else
                actionShowPwd->setChecked( false );
        }
        else
                actionShowPwd->setChecked( false );*/
    // TODO: Сделать показ/скрытие паролей
}
void MainWindow::on_actionEditEdit_triggered()
{
        //editPwd( NULL );
}
void MainWindow::on_actionSave_triggered()
{
    if ( fileName.isEmpty() )
        saveAs();
    else
        saveDatabase();
}
void MainWindow::on_actionSaveAs_triggered()
{
    saveAs();
}
void MainWindow::on_actionEditDel_triggered()
{
    /*QTreeWidgetItem *item = mainTree->selectedItems()[0];
    QString itemStr = "";
    if ( item->data( 0, Qt::UserRole ).toInt() == 0 )
        itemStr = trUtf8("запись");
    else
        itemStr = trUtf8("папку");

    QString itemName = item->data( 0, Qt::DisplayRole ).toString();
    int questresult = QMessageBox::question( this, trUtf8("Удаление"),
                               trUtf8("Вы действительно хотите удалить %1 с именем %2").arg( itemStr, itemName ),
                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
    if ( questresult == QMessageBox::Yes )
    {
        if ( item->parent() )
            item->parent()->removeChild( item );
        else
            item->~QTreeWidgetItem();

        setModif ( true );
    }*/
    // TODO: Сделать удаление записи/папки
}
void MainWindow::on_actionOpen_triggered()
{
    if ( !querySave() )
        return;

    QFileDialog openDlg ( this, trUtf8("Выберите файл для загрузки") );
    openDlg.setAcceptMode( QFileDialog::AcceptOpen );
    openDlg.setDefaultSuffix( "cxml" );
    openDlg.setNameFilters( QStringList() << trUtf8("Шифрованные XML файлы (%1)").arg( "*.cxml" )
                            << trUtf8("Файлы Cryptolog (%1)").arg("*.crypt")
                            << trUtf8("Любые файлы (%1)").arg("*") );
    if ( openDlg.exec() ) {
        if ( openDlg.selectedFiles().count() == 0 )
            return;
        newDatabase();
        fileName = openDlg.selectedFiles()[0];
        loadDatabase();
        setModif ( false );
    }

}
void MainWindow::on_actionChangeCurrentPassword_triggered()
{
    changeCurrentPassword();
}
void MainWindow::on_actionCopyToClipboard_triggered()
{
    /*if ( mainTree->selectedItems().count() == 0 )
        return;
    QTreeWidgetItem *current = mainTree->selectedItems()[0];

    QClipboard *clipboard = QApplication::clipboard();
    BasketUtils butil;
    QString pwd = butil.decrypt( current->data(2, Qt::UserRole).toString(), mainPassword.toHex());
    clipboard->setText( pwd );*/
    // TODO: Сделать копирование пароля в буфер обмена
}
void MainWindow::on_actionViewExpand_triggered()
{
    setTreeExpanded ( true );
}
void MainWindow::on_actionViewUnExpand_triggered()
{
    setTreeExpanded ( false );
}
void MainWindow::on_actionHelpAbout_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}
void MainWindow::on_actionHelpAboutQt_triggered()
{
    QMessageBox::aboutQt(this, trUtf8("Информация об установленной версии Qt"));
}
void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog sd(this);
    QString oldIdent = databaseIdentifier;
    sd.setIdentInfo(databaseIdentifier, lastModified);
    //int result =
    if ( sd.exec() == QDialog::Accepted ) {
        QSettings set;
        dontCloseApp = set.value(tr("DontCloseApp"), false).toBool();
        if ( sd.getIdent() != oldIdent ) {
            databaseIdentifier = sd.getIdent();
            setModif(true);
        }
    }
}
void MainWindow::on_actionCopyLogin_triggered()
{
    /*if ( mainTree->selectedItems().count() == 0 )
        return;
    QTreeWidgetItem *current = mainTree->selectedItems()[0];

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(current->data(1, Qt::DisplayRole).toString());*/
    // TODO: Копирование логина в буфер обмена
}
void MainWindow::trayIconClose()
{
    dontCloseApp = false;
    close();
}
