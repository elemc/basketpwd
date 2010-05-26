#include "editfolderdialog.h"
//#include "ui_editfolderdialog.h"
#include <QBuffer>
#include <QMessageBox>

editFolderDialog::editFolderDialog(QWidget *parent, QTreeWidgetItem *editedFolder) :
    QDialog(parent){
    m_ui.setupUi(this);

    m_ui.groupBox->hide();
    layout()->setSizeConstraint( QLayout::SetFixedSize );
    m_ui.groupBox->setVisible( false );

    hideEditedFolder = editedFolder;

    connect ( m_ui.treeWidget, SIGNAL (itemSelectionChanged()), this, SLOT(currentItemChanged()));
}

void editFolderDialog::changeEvent(QEvent *e)
{
    switch(e->type()) {
    case QEvent::LanguageChange:
        m_ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void editFolderDialog::parseTreeItem ( QTreeWidgetItem *parent, QTreeWidgetItem *subitem )
{
    if ( subitem->data( 0, Qt::UserRole ).toInt() == 0 )
        return;

    QTreeWidgetItem *newitem = new QTreeWidgetItem();//subitem->clone();
    newitem->setData( 0, Qt::DisplayRole, subitem->data( 0, Qt::DisplayRole ) );
    hash[subitem] = newitem;
    //newitem->setExpanded( true );

    if ( subitem == hideEditedFolder )
        return;

    if ( parent )
        parent->addChild( newitem );
    else
        m_ui.treeWidget->addTopLevelItem( newitem );
    newitem->setExpanded( true );

    for ( int i = 0; i < subitem->childCount(); i++ ) {
        parseTreeItem (newitem, subitem->child( i ) );
    }

    /*if ( newitem->childCount() > 0 )
        newitem->setExpanded( true );*/

}

void editFolderDialog::on_buttonBox_accepted()
{
    if ( m_ui.lineEditFolderName->text().isEmpty() )
        return;

    accept();
}

void editFolderDialog::on_buttonBox_rejected()
{
    reject();
}

void editFolderDialog::setMainTree( QTreeWidget *mainTree, QTreeWidgetItem *selectedFolder )
{
    QTreeWidget *tree = m_ui.treeWidget;
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

    for ( int i = 0; i < mainTree->topLevelItemCount(); i++ ) {
        parseTreeItem( root, mainTree->topLevelItem(i) );
    }

    if ( selectedFolder ) {
        foreach ( QTreeWidgetItem *item, hash.keys() )
        {
            QTreeWidgetItem *valueItem = hash[item];
            if ( (item == selectedFolder) && (valueItem) ) {
                valueItem->setSelected( true );
                tree->setCurrentItem( valueItem );

                currentItemChanged();
            }
        }
    }
    else {
        root->setSelected( true );
        tree->setCurrentItem( root );
        currentItemChanged();
    }
}

void editFolderDialog::currentItemChanged()
{
    /*if ( m_ui.treeWidget->selectedItems().count()==0 ) {
        m_ui.lineEditParent->setText( trUtf8("(корневой)") );
        return;
    }

    QTreeWidgetItem *selItem = m_ui.treeWidget->selectedItems()[0];*/
    if ( !m_ui.treeWidget->currentItem() ) {
        m_ui.lineEditParent->setText( trUtf8("(корневой)") );
        return;
    }

    QTreeWidgetItem *selItem = m_ui.treeWidget->currentItem();
    if ( selItem == root )
        m_ui.lineEditParent->setText( trUtf8("(корневой)") );
    else
        m_ui.lineEditParent->setText( selItem->data( 0, Qt::DisplayRole ).toString() );
}

void editFolderDialog::setFolderName( QString value )
{
    m_ui.lineEditFolderName->setText( value );
}

QTreeWidgetItem *editFolderDialog::getParent()
{
    if ( m_ui.treeWidget->selectedItems().count()==0 )
        return NULL;
    if ( m_ui.treeWidget->selectedItems()[0] == root )
        return NULL;
    else {
        foreach ( QTreeWidgetItem *value, hash.values() ) {
            if ( m_ui.treeWidget->selectedItems()[0] == value )
                return hash.key( value );
        }
    }
    return NULL;
}

QString editFolderDialog::getFolderName()
{
    return m_ui.lineEditFolderName->text();
}
