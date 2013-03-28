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

private:
    QLabel              *labelInformation;
    QLineEdit           *lePassword;
    //QDialogButtonBox    *buttonBox;
    QVBoxLayout         *vBoxElements;
    QHBoxLayout         *hBox;
    QPushButton         *ok;
    QPushButton         *cancel;

private slots:
    void buttonBox_accept();
    void buttonBox_reject();

signals:
    void passwordAccept( const QString &password );
    void passwordReject();
};

#endif


