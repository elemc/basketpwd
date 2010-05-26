#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    QSettings set;
    QString defaultPath = set.value(tr("PathToDef"), QString(QDir::currentPath())).toString();
    m_ui->lineEditDefaultPath->setText(defaultPath);

}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDialog::on_pushButton_clicked()
{
    QString oldPath = m_ui->lineEditDefaultPath->text();
    QString dir = QFileDialog::getExistingDirectory(this, trUtf8("Выберите папку"),
                                                     oldPath,
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if ( !dir.isEmpty() )
        m_ui->lineEditDefaultPath->setText(dir);
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    if ( m_ui->buttonBox->buttonRole( button ) == QDialogButtonBox::AcceptRole ) {
        QString defPath = m_ui->lineEditDefaultPath->text();
        QSettings set;
        QDir dir(defPath);
        if ( dir.exists() ) {


            set.setValue(tr("PathToDef"), defPath);
        }

        else {
            int result = QMessageBox::warning(this, trUtf8("Предупреждение"),
                                 trUtf8("Заданный путь не существует.\nВы уверены, что хотите сохранить настройки?"),
                                 QMessageBox::Yes | QMessageBox::No);
            if ( result == QMessageBox::No )
                return;
            else {
                set.setValue(tr("PathToDef"), defPath);
            }
        }

        set.setValue(tr("DontCloseApp"), m_ui->checkBoxDontClose->checkState());
        accept();
    }
    else if ( m_ui->buttonBox->buttonRole( button ) == QDialogButtonBox::RejectRole )
        reject();

}
