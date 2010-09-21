#include "changepassword.h"
#include "ui_changepassword.h"

ChangePassword::ChangePassword(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ChangePassword)
{
    m_ui->setupUi(this);
}
ChangePassword::~ChangePassword()
{
    delete m_ui;
}
void ChangePassword::changeEvent(QEvent *e)
{
    switch(e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ChangePassword::on_buttonBox_accepted()
{
    bool itsOk = ( m_ui->lineEditNewPassword->text() == m_ui->lineEditConfirmPassword->text() )
                 && !m_ui->lineEditNewPassword->text().isEmpty();

    if ( itsOk )
        accept();
}

void ChangePassword::on_buttonBox_rejected()
{
    reject();
}

QString ChangePassword::getCurrentPassword()
{
    return m_ui->lineEditCurrentPassword->text();
}

QString ChangePassword::getNewPassword()
{
    return m_ui->lineEditNewPassword->text();
}

void ChangePassword::setNewPassword( bool isNewPassword )
{
    m_ui->labelCurrentPassword->setVisible( !isNewPassword );
    m_ui->lineEditCurrentPassword->setVisible( !isNewPassword );
}

