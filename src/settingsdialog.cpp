#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "basketcommon.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    QString defPath = BasketCommon::getDefaultDirectory();

    QSettings set;
    QString defaultPath = set.value(tr("PathToDef"), QString(defPath)).toString();
    int csi             = set.value(tr("DontCloseApp"), m_ui->checkBoxDontClose->checkState()).toInt();
    int csort_raw       = set.value(tr(SORTING), true).toInt();

#if defined(Q_WS_X11)
    QString fallback_theme_name = "";
#else
    QString fallback_theme_name = DEFAULT_ICON_THEME;
#endif
    QString iconTheme   = set.value("IconTheme", QString(fallback_theme_name)).toString();

    Qt::CheckState cs = csi == 0 ? Qt::Unchecked : Qt::Checked;
    Qt::CheckState csort = csort_raw == 0? Qt::Unchecked : Qt::Checked;
    m_ui->checkBoxDontClose->setCheckState(cs);
    m_ui->lineEditDefaultPath->setText(defaultPath);
    m_ui->checkBoxSort->setCheckState(csort);

    /* Colors */
    selectedColor       = set.value(tr("ItemColor"), QColor(Qt::darkBlue)).value<QColor>();
    setColorToItLabel(selectedColor);

    /* Icon theme */
#if defined(Q_WS_X11)
    m_ui->comboBoxIconTheme->addItem( trUtf8("Системная"), QString("") ); 
#endif
    m_ui->comboBoxIconTheme->addItem( trUtf8("Современная (oxygen)"), QString("oxygen-internal") ); 
    m_ui->comboBoxIconTheme->addItem( trUtf8("Классическая"), QString("classic-internal") );
    m_ui->comboBoxIconTheme->setCurrentIndex( getIconThemeIndex( iconTheme) );

    /* Mac OS X fast passwords */
#if defined(Q_WS_MAC)
    QString fastPlace = set.value(QString("FastPasswordsPlace"), QString("dock")).toString();

    m_ui->comboBoxFastPasswords->addItem ( trUtf8("Dock + Tray"), QString("dock_tray") );
    m_ui->comboBoxFastPasswords->addItem ( trUtf8("Dock"), QString("dock") );
    m_ui->comboBoxFastPasswords->addItem ( trUtf8("Tray"), QString("tray") );

    m_ui->comboBoxFastPasswords->setCurrentIndex( getFastPlace ( fastPlace ) );
#else
    m_ui->groupBoxOSSpecified->hide();
#endif
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
            int result = QMessageBox::warning(this, 
                        trUtf8("Предупреждение"),
                        trUtf8("Заданный путь не существует.\nВы уверены, что хотите сохранить настройки?"),
                        QMessageBox::Yes | QMessageBox::No);
            if ( result == QMessageBox::No )
                return;
            else {
                set.setValue(tr("PathToDef"), defPath);
            }
        }

        set.setValue("DontCloseApp", m_ui->checkBoxDontClose->checkState());
        set.setValue(SORTING, m_ui->checkBoxSort->checkState());
        set.setValue("ItemColor", selectedColor);
        QString themeName = m_ui->comboBoxIconTheme->itemData( 
                            m_ui->comboBoxIconTheme->currentIndex() ).toString();
        set.setValue("IconTheme", themeName );
#if defined(Q_WS_MAC)
        QString fastPlace = m_ui->comboBoxFastPasswords->itemData(
                            m_ui->comboBoxFastPasswords->currentIndex() ).toString();
        set.setValue("FastPasswordsPlace", fastPlace);
#endif

        QIcon::setThemeName( themeName );

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
int SettingsDialog::getIconThemeIndex (const QString &themeName) const
{
    int it_idx = -1;
    if ( themeName == "" )
        it_idx = 0;
    else if ( themeName == "oxygen-internal" )
        it_idx = 1;
    else if ( themeName == "classic-internal" )
        it_idx = 2;

#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
    if ( it_idx == -1 )
        it_idx = 1;

    it_idx -= 1; // trick
#endif
    if ( it_idx < 0 )
        it_idx = 0;

    return it_idx;
}
#if defined(Q_WS_MAC)
int SettingsDialog::getFastPlace ( const QString &place ) const
{
    int idx = -1;
    if ( place == QString("dock_tray") )
        idx = 0;
    else if ( place == QString("dock") )
        idx = 1;
    else if ( place == QString("tray") )
        idx = 2;
    else
        idx = 1;
    return idx;
}
#endif
