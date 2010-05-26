#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H

#include <QtGui/QDialog>

namespace Ui {
    class ChangePassword;
}

class ChangePassword : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ChangePassword)
public:
    explicit ChangePassword(QWidget *parent = 0);
    virtual ~ChangePassword();
    void setNewPassword ( bool isNewPassword );
    QString getCurrentPassword();
    QString getNewPassword();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ChangePassword *m_ui;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    //void checkTextChanged();
};

#endif // CHANGEPASSWORD_H
