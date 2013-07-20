#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include <QAbstractButton>
#include <QDateTime>
#include <QColorDialog>
#include "config.h"

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
    QColor selectedColor;

    void setColorToItLabel(QColor &clr);
    int getIconThemeIndex(const QString &themeName) const;
#if defined(Q_WS_MAC)
    int getFastPlace( const QString &place ) const;
#endif

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_pushButtonSelectDir_clicked();
    void on_pushButtonSelectColor_clicked();
};

#endif // SETTINGSDIALOG_H
