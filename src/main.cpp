#include <QApplication>
#include "mainwindow.h"
#include <QMessageBox>
#include <QTranslator>
#include <QTextCodec>
//
#define PROGRAM_VERSION "0.3.7"

int main(int argc, char ** argv)
{
    QApplication app( argc, argv );

    QTextCodec *rusCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(rusCodec);

    QTranslator qtTrans;
    if ( QLocale::system().name().toLower() == QObject::tr("ru_ru") ) {
        qtTrans.load(":/lang/qt_" + QLocale::system().name(), app.applicationDirPath());
    }
    else {
        qtTrans.load(":/lang/basketpwd_en", app.applicationDirPath());
    }

    app.installTranslator(&qtTrans);


    qApp->setOrganizationName(QObject::trUtf8("Атис Сервис"));
    qApp->setOrganizationDomain(QObject::tr("atisserv.ru"));
    qApp->setApplicationName(QObject::trUtf8("Корзинка паролей"));
    qApp->setApplicationVersion(QString(PROGRAM_VERSION));

    MainWindow win;

    bool showInTray = false;
    for (int i = 1; i < argc; i++) {
        if ( (QString(argv[i]) == QString("--tray-icon")) ||
             (QString(argv[i]) == QString("-ti")) ) {
            showInTray = true;
        }
    }

    if ( showInTray )
        win.hide();
    else
        win.show();

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
    return app.exec();
}
