#include "aboutdialog.h"
#include <QUrl>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent){
    m_ui.setupUi(this);

    m_ui.labelCaption->setText(captionLabel());


}

void AboutDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void AboutDialog::on_buttonBox_accepted()
{
    accept();
}

void AboutDialog::on_buttonBox_rejected()
{
    reject();
}

QString AboutDialog::captionLabel()
{
#if QT_VERSION >= 0x040400
    QString app_ver = qApp->applicationVersion();
#else
    QString app_ver = QString(VER);
#endif
    QString captLabel = QString(trUtf8(
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\n\">"
"<html>"
"<head>"
"<meta name=\"qrichtext\" content=\"1\" />"
"<style type=\"text/css\">"
"p, li { white-space: pre-wrap; }"
"</style>"
"</head>"
"<body style=\" font-family:'Tahoma'; font-size:9pt; font-weight:400; font-style:normal;\">"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
"<span style=\" font-size:11pt; font-weight:600;\">"
"Корзинка паролей v%1"
"</span>"
"</p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
"Программа для удобного хранения паролей."
"</p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
"Пароли хранятся в зашифрованном файле."
"</p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
"Программа может использоваться, как"
"</p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
"стационарно, так и на переносном устройстве."
"</p>"
"</body>"
"</html>"
).arg(app_ver));
    return captLabel;
}

void AboutDialog::on_label_4_linkActivated(QString link)
{
    QUrl url(link);
    QDesktopServices serv;
    serv.openUrl(url);
}
