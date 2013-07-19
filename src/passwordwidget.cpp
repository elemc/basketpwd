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
    _passwordWidgetType = PasswordWidget::AskPassword;
    initialWidget();
}

PasswordWidget::~PasswordWidget () 
{
    clearWidget();
}

// Private slots
void PasswordWidget::buttonBox_accept()
{
    if ( _passwordWidgetType == PasswordWidget::ChangePassword )
        _currentPassword = leCurrentPassword->text();
    emit passwordAccept( lePassword->text() );
    clearPasswordField ();
}
void PasswordWidget::buttonBox_reject()
{
    emit passwordReject();
    clearPasswordField ();
}
void PasswordWidget::checkDoublePasswords( const QString &pwd )
{
    Q_UNUSED(pwd)
    bool access = false;
    QString leap = leAgainPassword->text();
    QString lep = lePassword->text();
    QString lecp = leCurrentPassword->text();
    
    if ( leap == lep && !lep.isEmpty() ) {
        access = true;
        if ( _passwordWidgetType == PasswordWidget::ChangePassword )
            access &= !lecp.isEmpty();
            
    }
        
    ok->setEnabled( access );
    if ( access ) {
        connect( lePassword, SIGNAL(returnPressed()), this, SLOT(buttonBox_accept()) );
        connect( leAgainPassword, SIGNAL(returnPressed()), this, SLOT(buttonBox_accept()) );
    }
    else {
        disconnect( lePassword, SIGNAL(returnPressed()) );
        disconnect( leAgainPassword, SIGNAL(returnPressed()) );
    }
}

// Public methods
void PasswordWidget::setPasswordWidgetType ( PasswordWidgetType pwt )
{
    if ( _passwordWidgetType == pwt )
        return;
    _passwordWidgetType = pwt;
    clearWidget();
    initialWidget();
}

PasswordWidget::PasswordWidgetType PasswordWidget::passwordWidgetType () const
{
    return _passwordWidgetType;
}

void PasswordWidget::clearPasswordField ()
{
    lePassword->clear();
    leAgainPassword->clear();
    leCurrentPassword->clear();
}

QString PasswordWidget::currentPassword() const
{
    return _currentPassword;
}

// Private methods
void PasswordWidget::clearWidget ()
{
    delete labelInformation;
    labelInformation = 0;

    delete lePassword;
    lePassword = 0;

    delete leAgainPassword;
    leAgainPassword = 0;

    delete ok;
    ok = 0;

    delete cancel;
    cancel = 0;

    delete vBoxElements;
    vBoxElements = 0;
    
    delete hBox;
    hBox = 0;
}
void PasswordWidget::initialWidget ()
{
    hBox = new QHBoxLayout;
    vBoxElements = new QVBoxLayout;

    labelInformation = new QLabel ( this );
    labelInformation->setText( trUtf8("Введите пароль для доступа к файлу") );

    leCurrentPassword = new QLineEdit ( this );
    leCurrentPassword->setEchoMode( QLineEdit::Password );
    lePassword = new QLineEdit( this );
    lePassword->setEchoMode( QLineEdit::Password );
    leAgainPassword = new QLineEdit ( this );
    leAgainPassword->setEchoMode( QLineEdit::Password );

    if ( _passwordWidgetType == PasswordWidget::AskPassword ) {
        connect( lePassword, SIGNAL(returnPressed()), this, SLOT(buttonBox_accept()) );
        leAgainPassword->hide();
        leCurrentPassword->hide();
    }
    else if ( _passwordWidgetType == PasswordWidget::SetPassword ) {
        leCurrentPassword->hide();
    }

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

    QSpacerItem *verticalSpacer1    = new QSpacerItem ( 20, 20, QSizePolicy::Minimum, 
                                                                QSizePolicy::Expanding );
    QSpacerItem *verticalSpacer2    = new QSpacerItem ( 20, 20, QSizePolicy::Minimum, 
                                                                QSizePolicy::Expanding );
    QSpacerItem *horizontalSpacer1  = new QSpacerItem ( 20, 20, QSizePolicy::Expanding, 
                                                                QSizePolicy::Minimum );
    QSpacerItem *horizontalSpacer2  = new QSpacerItem ( 20, 20, QSizePolicy::Expanding, 
                                                                QSizePolicy::Minimum );

    vBoxElements->addSpacerItem( verticalSpacer1 ); 
    vBoxElements->addWidget( labelInformation );

    if ( _passwordWidgetType == PasswordWidget::AskPassword )
        vBoxElements->addWidget( lePassword );
    else {
        if ( _passwordWidgetType == PasswordWidget::ChangePassword ) {
            vBoxElements->addWidget( leCurrentPassword );
        }
        vBoxElements->addWidget( lePassword );
        vBoxElements->addWidget( leAgainPassword );

        // connect it to check slot
        connect ( lePassword, SIGNAL(textChanged(QString)), this, SLOT(checkDoublePasswords(QString)) );
        connect ( leAgainPassword, SIGNAL(textChanged(QString)), this, SLOT(checkDoublePasswords(QString)) );
        connect ( leCurrentPassword, SIGNAL(textChanged(QString)),
                    this, SLOT( checkDoublePasswords(QString) ) );
        checkDoublePasswords(QString());
    }
    vBoxElements->addLayout( buttonLayout );
    vBoxElements->addSpacerItem( verticalSpacer2 );

    hBox->addSpacerItem( horizontalSpacer1 );
    hBox->addLayout( vBoxElements );
    hBox->addSpacerItem( horizontalSpacer2 );

    setLayout( hBox );
}

