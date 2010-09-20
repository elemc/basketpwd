#include "editpwddialog.h"
//
editPwdDialog::editPwdDialog( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
    setupUi(this);

    groupBox->hide();
    //layout()->setSizeConstraint( QLayout::SetFixedSize );
    //groupBox->setVisible( false );

    //connect ( treeWidget, SIGNAL (itemSelectionChanged()), this, SLOT(currentItemChanged()));
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

void editPwdDialog::setModel(BasketModel *source_model, QModelIndex index)
{
    model = source_model;
    mapper->setModel(model);
    mapper->setCurrentModelIndex(index);

    // связываем
    mapper->addMapping(lineEditName, 0);
    mapper->addMapping(lineEditLogin, 1);
    //mapper->addMapping(lineEditPassword, 2);


}

//

void editPwdDialog::on_buttonBox_accepted()
{
    /*if ( lineEditName->text().isEmpty() )
        return;
    elementName = lineEditName->text();
    elementLogin = lineEditLogin->text();
    elementPassword = lineEditPassword->text();*/
    accept();
}

void editPwdDialog::on_buttonBox_rejected()
{
    reject();
}

void editPwdDialog::currentItemChanged()
{
    /*if ( treeWidget->selectedItems().count()==0 ) {
        lineEditLocation->setText( trUtf8("(корневой)") );
        return;
    }

    QTreeWidgetItem *selItem = treeWidget->selectedItems()[0];
    if ( selItem == root )
        lineEditLocation->setText( trUtf8("(корневой)") );
    else
        lineEditLocation->setText( selItem->data( 0, Qt::DisplayRole ).toString() );*/
}
/*void editPwdDialog::setElement(QString eName, QString eLogin, QString ePwd)
{
    lineEditName->setText( eName );
    lineEditLogin->setText ( eLogin );
    lineEditPassword->setText ( ePwd );
}

void editPwdDialog::parseTreeItem ( QTreeWidgetItem *parent, QTreeWidgetItem *subitem )
{
    if ( subitem == NULL )
        return;
    if ( subitem->data( 0, Qt::UserRole ).toInt() == 0 )
        return;

    QTreeWidgetItem *newitem = new QTreeWidgetItem();//subitem->clone();
    newitem->setData( 0, Qt::DisplayRole, subitem->data( 0, Qt::DisplayRole ) );
    hash[subitem] = newitem;

    if ( parent )
        parent->addChild( newitem );
    else
        treeWidget->addTopLevelItem( newitem );

    newitem->setExpanded( true );

    int childCount = subitem->childCount();
    for ( int i = 0; i < childCount; i++ ) {
        parseTreeItem (newitem, subitem->child( i ) );
    }

}

void editPwdDialog::setMainTree( QTreeWidget *mainTree, QTreeWidgetItem *selectedFolder )
{
    QTreeWidget *tree = treeWidget;
    tree->clear();
    tree->setColumnCount( 1 );
    tree->setSortingEnabled( true );
    tree->sortByColumn(0, Qt::AscendingOrder );
    tree->setHeaderLabels( QStringList() << trUtf8("Наименование") );
    hash.clear();
    //Заполняем дерево только папками
    // Создаем корневую ветвь, все наследуют от нее
    root = new QTreeWidgetItem ();
    root->setData( 0, Qt::DisplayRole, trUtf8("<Корневая ветвь>") );
    tree->addTopLevelItem( root );
    root->setExpanded( true );
    int topLevelItemCount = mainTree->topLevelItemCount();
    for ( int i = 0; i < topLevelItemCount; i++ ) {
        parseTreeItem( root, mainTree->topLevelItem(i) );
    }
    if ( selectedFolder ) {
        foreach ( QTreeWidgetItem *item, hash.keys() )
        {
            QTreeWidgetItem *valueItem = hash[item];
            if ( (item == selectedFolder) && (valueItem) )
                valueItem->setSelected( true );
        }
    }
    else
        root->setSelected( true );
}

QTreeWidgetItem *editPwdDialog::getParent()
{
    if ( treeWidget->selectedItems().count()==0 )
        return NULL;
    if ( treeWidget->selectedItems()[0] == root )
        return NULL;
    else {
        foreach ( QTreeWidgetItem *value, hash.values() ) {
            if ( treeWidget->selectedItems()[0] == value )
                return hash.key( value );
        }
    }
    return NULL;
}
*/
