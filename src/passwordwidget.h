// -*- C++ -*-
/* ------------------------------------ */
/* C++ class header (QWidget)           */
/* Author: Alexei Panov <me@elemc.name> */
/* ------------------------------------ */

/* Name: (PasswordWidget)
   Description: widget for ask password 
*/

#ifndef PASSWORDWIDGET_H
#define PASSWORDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>

class PasswordWidget : public QWidget
{
Q_OBJECT
public:
    PasswordWidget ( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~PasswordWidget ();

    enum PasswordWidgetType {   AskPassword, 
                                SetPassword, 
                                ChangePassword };

    void setPasswordWidgetType ( PasswordWidgetType pwt );
    PasswordWidgetType passwordWidgetType() const;
    void clearPasswordField ();
    QString currentPassword () const;

private:
    QLabel              *labelInformation;
    QLineEdit           *leCurrentPassword;
    QLineEdit           *lePassword;
    QLineEdit           *leAgainPassword;
    QVBoxLayout         *vBoxElements;
    QHBoxLayout         *hBox;
    QPushButton         *ok;
    QPushButton         *cancel;
    QString             _currentPassword;

    PasswordWidgetType  _passwordWidgetType;
    void initialWidget();
    void clearWidget();

private slots:
    void buttonBox_accept();
    void buttonBox_reject();
    void checkDoublePasswords( const QString &pwd );

signals:
    void passwordAccept( const QString &password );
    void passwordReject();
};

#endif


