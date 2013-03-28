// -*- C++ -*-
/* ------------------------------------ */
/* C++ class source (QWidget)           */
/* Author: Alexei Panov <me@elemc.name> */
/* ------------------------------------ */

/* Name: (PasswordWidget)
   Description: 
*/

#include "passwordwidget.h"

PasswordWidget::PasswordWidget(QWidget *parent, Qt::WindowFlags f) : 
	QWidget(parent, f)
{
    hBox = new QHBoxLayout;
    vBoxElements = new QVBoxLayout;

    labelInformation = new QLabel ( this );
    labelInformation->setText( trUtf8("Введите пароль для доступа к файлу") );

    lePassword = new QLineEdit( this );
    lePassword->setEchoMode( QLineEdit::Password );
    connect( lePassword, SIGNAL(returnPressed()), this, SLOT(buttonBox_accept()) );

    /*buttonBox = new QDialogButtonBox ( this );
    QPushButton *ok                 = buttonBox->addButton( QDialogButtonBox::Ok );
    QPushButton *cancel             = buttonBox->addButton( QDialogButtonBox::Cancel );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(buttonBox_accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(buttonBox_reject()));*/
    ok                              = new QPushButton ( trUtf8("ОК") );
    cancel                          = new QPushButton ( trUtf8("Отмена") );
#ifndef Q_WS_MAC
    ok->setIcon( QIcon::fromTheme( "dialog-ok" ) );
    cancel->setIcon( QIcon::fromTheme( "dialog-cancel" ) );
#endif
    ok->setDefault( true );
    ok->setAutoDefault( true );
    connect( ok, SIGNAL(clicked()), this, SLOT(buttonBox_accept()) );
    connect( cancel, SIGNAL(clicked()), this, SLOT(buttonBox_reject()) );

    QHBoxLayout *buttonLayout       = new QHBoxLayout;
    buttonLayout->addWidget( ok );
    buttonLayout->addWidget( cancel );

    QSpacerItem *verticalSpacer1    = new QSpacerItem ( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    QSpacerItem *verticalSpacer2    = new QSpacerItem ( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    QSpacerItem *horizontalSpacer1  = new QSpacerItem ( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QSpacerItem *horizontalSpacer2  = new QSpacerItem ( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    vBoxElements->addSpacerItem( verticalSpacer1 ); 
    vBoxElements->addWidget( labelInformation );
    vBoxElements->addWidget( lePassword );
    vBoxElements->addLayout( buttonLayout );
    vBoxElements->addSpacerItem( verticalSpacer2 );

    hBox->addSpacerItem( horizontalSpacer1 );
    hBox->addLayout( vBoxElements );
    hBox->addSpacerItem( horizontalSpacer2 );

    setLayout( hBox );
}

PasswordWidget::~PasswordWidget () 
{
    delete labelInformation;
    labelInformation = 0;

    delete lePassword;
    lePassword = 0;

    //delete buttonBox;
    //buttonBox = 0;

    delete vBoxElements;
    vBoxElements = 0;
    
    delete hBox;
    hBox = 0;
}

// Private slots
void PasswordWidget::buttonBox_accept()
{
    emit passwordAccept( lePassword->text() );
}
void PasswordWidget::buttonBox_reject()
{
    emit passwordReject();
}
