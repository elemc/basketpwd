#include "mainwindow.h"
#include "changepassword.h"
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
    tree->setDragEnabled(true);
    tree->setAcceptDrops(true);
    tree->setDropIndicatorShown(true);
    setCentralWidget(tree);

    connect ( tree->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentItemChanged(QModelIndex,QModelIndex)));
    connect ( model, SIGNAL(modelDataChanged()), this, SLOT(onModelDataChanged()) );
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

        connect ( this->actionViewExpand, SIGNAL(triggered()), tree, SLOT(expandAll()));
        connect ( this->actionViewUnExpand, SIGNAL(triggered()), tree, SLOT(collapseAll()));
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

    // TODO: Сделать очистку модели
    createTreeWidget();
    mainPassword = QByteArray();
    model->setUpNewModel( mainPassword );

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
    /*QString ext = fileName.right(4);

    QDomDocument    doc;
    QDomElement	root = doc.createElement( trUtf8("basket-passwords") );
    root.setAttribute(QString("ident"), databaseIdentifier);

    // Сохраняем текущий параметр даты/времени
    lastModified = QDateTime::currentDateTime();
    root.setAttribute(QString("modified"), lastModified.toString(DATE_TIME_FORMAT));

    doc.appendChild( root );

    for ( int i = 0; i < mainTree->topLevelItemCount(); i++ )
            addTreeItemToDom( &root, mainTree->topLevelItem(i) );
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
    BasketUtils butil;*/
    QByteArray buf = model->modelDataToXML();
    if ( buf.isEmpty() ) {
        QMessageBox::critical(this,
                              tr("Ошибка сохранения данных"),
                              tr("Произошла ошибка при попытке составить XML-файл, обратитесь к разработчику!"));
        return;
    }

    QFile filew ( fileName );
    bool result = filew.open( QIODevice::WriteOnly );
    if ( !result ) {
        QMessageBox::critical( this,
                               tr("Ошибка доступа к файлу"),
                               tr("Ошибка открытия файла %1 для записи").arg( fileName ) );
        return;
    }

    filew.write( buf );
    filew.close();

    setModif ( false );
}
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
        tree->resizeColumnToContents(0);
        tree->resizeColumnToContents(1);
        tree->resizeColumnToContents(2);
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
void MainWindow::treeItemDoubleClicked (QModelIndex index)
{
    if ( !index.isValid() )
        return;

    /*if ( item->data( 0, Qt::UserRole ).toInt() == 0 )
        editPwd ( item );*/
    // TODO: Сделать активацию, открытие диалога редактирования

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
    QString id = QString("%1 - %2 (%3) %4").arg(winTitle).arg(model->identifier()).arg(model->lastModifiedStr()).arg(strModif);
    setWindowTitle(id.trimmed());
    actionSave->setEnabled( modificator );
}
void MainWindow::changeCurrentPassword()
{
    ChangePassword cpDlg( this );
    if ( mainPassword.isNull() )
        cpDlg.setNewPassword( true );
    if ( cpDlg.exec() ) {
        if ((( cpDlg.getCurrentPassword().isEmpty() ) && ( mainPassword.isNull() )) || ( hashPassword(cpDlg.getCurrentPassword()) == mainPassword ) ) {
            QByteArray tempPassword = hashPassword(cpDlg.getNewPassword());
            if ( model->changePassword( tempPassword.toHex() ) )
                mainPassword = tempPassword;
            setModif( true );
            allowActions( true );
        }
    }
    else if ( mainPassword.isNull() )
        allowActions ( false );
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
    actionEditEdit->setEnabled( isAct );
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
    return BasketUtils::hashPassword(pwd);
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
void MainWindow::addItemToModel(bool isFolder)
{
    QModelIndex idx = tree->selectionModel()->currentIndex();
    if ( !idx.isValid() )
        model->insertRow(model->rowCount(), QModelIndex(), isFolder);
    else if ( model->indexIsFolder(idx) ){
        model->insertRow(idx.row(), idx, isFolder);
    }
    else {
        model->insertRow(idx.row(), idx.parent(), isFolder);
    }
}

// Слоты
// File actions
void MainWindow::on_actionExit_triggered()
{
    close();
}
void MainWindow::on_actionNew_triggered()
{
    if ( querySave() )
        newDatabase( true );
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

// Edit actions
void MainWindow::on_actionEditAddFolder_triggered()
{
    addItemToModel(true);
}
void MainWindow::on_actionEditAddPwd_triggered()
{
    addItemToModel(false);
}
void MainWindow::on_actionEditEdit_triggered()
{
    //editPwd( );
    if ( tree->currentIndex().isValid() ) {
        ChangePassword dlg(this);
        dlg.setNewPassword(true);
        if ( dlg.exec() == QDialog::Accepted ) {
            model->setPassword(tree->currentIndex(), dlg.getNewPassword());
        }
    }
}
void MainWindow::on_actionEditDel_triggered()
{
    QModelIndex idx = tree->selectionModel()->currentIndex();
    if ( !idx.isValid() )
        return;

    QString cap, t;
    if ( model->indexIsFolder(idx) ) {
        cap = tr("группы");
        t = tr("группу");
    }
    else {
        cap = tr("записи");
        t = tr("запись");
    }

    if ( QMessageBox::question(this,
                               tr("Удаление %1").arg(cap),
                               tr("Вы действительно хотите удалить %1").arg(t),
                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No )
        return;

    model->removeRow(idx.row(), idx.parent());
}
void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog sd(this);
    QString oldIdent = model->identifier();
    sd.setIdentInfo(model->identifier(), model->lastModified());
    if ( sd.exec() == QDialog::Accepted ) {
        QSettings set;
        dontCloseApp = set.value(tr("DontCloseApp"), false).toBool();
        if ( sd.getIdent() != oldIdent ) {
            model->setIdentifier(sd.getIdent());
            setModif(true);
        }
    }
}
void MainWindow::on_actionChangeCurrentPassword_triggered()
{
    changeCurrentPassword();
}
void MainWindow::on_actionCopyToClipboard_triggered()
{
    if ( tree->currentIndex().isValid() ) {
        QClipboard *clipboard = QApplication::clipboard();
        QString encPwd = model->cleanPassword(tree->currentIndex());
        clipboard->setText(encPwd);
    }
}
void MainWindow::on_actionCopyLogin_triggered()
{
    if ( tree->currentIndex().isValid() ) {
        QClipboard *clipboard = QApplication::clipboard();
        QString login = model->login(tree->currentIndex());
        clipboard->setText(login);
    }
}

// View actions
void MainWindow::on_actionShowPwd_triggered(bool checked)
{
    bool acceptDlg = false;
    QString tempPassword;

    if ( checked )
        tempPassword = QInputDialog::getText( this, trUtf8("Запрос пароля"),
                trUtf8("<b><p>Внимание! Потенциально опасно отображать пароли!</p>\n<p>Если Вы находитесь перед компьютером не один, нажмите кнопку \"Отмена\"</p></b>\nПодтвердите свой пароль:"),
                QLineEdit::Password, QString(), &acceptDlg );
    else {
        acceptDlg = true;
    }

    if ( (acceptDlg) ) {
        if ( (( checked ) && (hashPassword(tempPassword) == mainPassword) ) || ( !checked ) )
            model->setShowPasswords( checked );
        else {
            actionShowPwd->setChecked( false );
        }
    }
    else {
        actionShowPwd->setChecked( false );
    }
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
void MainWindow::trayIconClose()
{
    dontCloseApp = false;
    close();
}

void MainWindow::onModelDataChanged()
{
    setModif(true);
}
