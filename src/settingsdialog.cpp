#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

#ifdef Q_WS_WIN
    QString settingDirName = tr("Корзинка паролей"); 
#else
    QString settingDirName = ".basketpwd";
#endif
    
    QString defPath = QString(QDir::toNativeSeparators(QDir::homePath())  + QDir::separator() + settingDirName);

    QSettings set;
    QString defaultPath = set.value(tr("PathToDef"), QString(defPath)).toString();
    int csi             = set.value(tr("DontCloseApp"), m_ui->checkBoxDontClose->checkState()).toInt();
    int csort_raw       = set.value(tr(SORTING), true).toInt();
    Qt::CheckState cs = csi == 0 ? Qt::Unchecked : Qt::Checked;
    Qt::CheckState csort = csort_raw == 0? Qt::Unchecked : Qt::Checked;
    m_ui->checkBoxDontClose->setCheckState(cs);
    m_ui->lineEditDefaultPath->setText(defaultPath);
    m_ui->checkBoxSort->setCheckState(csort);

    /* Colors */
    selectedColor       = set.value(tr("ItemColor"), QColor(Qt::darkBlue)).value<QColor>();
    setColorToItLabel(selectedColor);
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
void SettingsDialog::setIdentInfo(QString ident, QDateTime modified)
{
    m_ui->lineEditIdent->setText(ident);
    m_ui->dateTimeEditModif->setDateTime(modified);
}
QString SettingsDialog::getIdent() const
{
    return m_ui->lineEditIdent->text();
}

void SettingsDialog::on_pushButtonSelectDir_clicked()
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
        set.setValue(tr(SORTING), m_ui->checkBoxSort->checkState());
        set.setValue(tr("ItemColor"), selectedColor);
        accept();
    }
    else if ( m_ui->buttonBox->buttonRole( button ) == QDialogButtonBox::RejectRole )
        reject();

}

void SettingsDialog::on_pushButtonSelectColor_clicked() 
{
  // FIXME: maybe QT bug here O_o
  QString tempDir = m_ui->lineEditDefaultPath->text();
  QColor sd_color = QColorDialog::getColor( selectedColor, this ); //, tr("Выберите цвет"));
  if ( sd_color.isValid() ) {
    selectedColor = sd_color;
    setColorToItLabel(selectedColor);
  }

  // FIXME: maybe QT bug here O_o
  if ( tempDir != m_ui->lineEditDefaultPath->text() )
    m_ui->lineEditDefaultPath->setText(tempDir);
}
void SettingsDialog::setColorToItLabel( QColor &clr )
{
  QString ls = tr("<html><head/><body><p><span style=\"color:%1;\">Цвет записей:</span></p></body></html>").arg(clr.name());
  m_ui->labelColorItems->setText(ls);
}
