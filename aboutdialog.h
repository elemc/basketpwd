#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "ui_aboutdialog.h"

class AboutDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(AboutDialog)
public:
    explicit AboutDialog(QWidget *parent = 0);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::AboutDialog m_ui;
    QString captionLabel();

private slots:
    void on_label_4_linkActivated(QString link);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // ABOUTDIALOG_H
