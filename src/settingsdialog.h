#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include <QAbstractButton>
#include <QDateTime>

#define SORTING "SortingEnabled"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(SettingsDialog)
public:
    explicit SettingsDialog(QWidget *parent = 0);
    virtual ~SettingsDialog();
    void setIdentInfo(QString ident, QDateTime modified);
    QString getIdent() const;

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SettingsDialog *m_ui;

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_pushButton_clicked();
};

#endif // SETTINGSDIALOG_H
