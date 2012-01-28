TEMPLATE = app
QT = gui \
    core \
    xml
#    network # перенос в плагины
CONFIG += qt \
    warn_on \
    debug_and_release \
    windows
!mac:DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui \
    ui/changepassword.ui \
    aboutdialog.ui \
    ui/settingsdialog.ui
HEADERS = src/mainwindow.h \
    src/basketutils.h \
    src/changepassword.h \
    aboutdialog.h \
    src/settingsdialog.h \
    model/basketbaseitem.h \
    model/basketmodel.h \
    src/coreplugin.h \
    src/pluginsloader.h
#    netsync/firstnetworksender.h \
#    netsync/qsyncclientlist.h \
#    netsync/syncclient.h \
#    netsync/udplistener.h \
#    netsync/udpnetworksocket.h
SOURCES = src/mainwindow.cpp \
    src/main.cpp \
    src/basketutils.cpp \
    src/changepassword.cpp \
    aboutdialog.cpp \
    src/settingsdialog.cpp \
    model/basketbaseitem.cpp \
    model/basketmodel.cpp \
    src/coreplugin.cpp \
    src/pluginsloader.cpp
 #   netsync/firstnetworksender.cpp \
 #   netsync/qsyncclientlist.cpp \
 #   netsync/syncclient.cpp \
 #   netsync/udplistener.cpp \
 #   netsync/udpnetworksocket.cpp
RESOURCES += icon-images.qrc
RC_FILE = basketpwd.rc
mac:RC_FILE = images/basketpwd.icns

QMAKE_INFO_PLIST = tools/Info.plist

ICON_SIZES = 128 192 256 32 36 48 64 72 96
ICON_PATH = 
ICON_EXTRA = 
INSTALL_ROOT=/usr/local

defineTest(CopyIcons) {
    extra = 
    path = 
    for(icon_size, ICON_SIZES) {
       ICON_EXTRA += install -D -m 0644 -p $${_PRO_FILE_PWD_}/images/icons/basketpwd_$${icon_size}x$${icon_size}.png $(INSTALL_ROOT)/usr/share/icons/hicolor/$${icon_size}x$${icon_size}/apps/basketpwd.png;
       ICON_PATH += /usr/share/icons/hicolor/$${icon_size}x$${icon_size}/apps
    }
    export ( ICON_EXTRA )
    export ( ICON_PATH )
}

win32 {
    CONFIG += static
    INCLUDEPATH += d:/devel/utils/openssl/openssl-shared-64/include
    LIBS += -ld:/devel/utils/openssl/openssl-shared-64/lib/libeay32
}
unix {
    LIBS += -lcrypto -lssl
    TARGET = basketpwd
    target.path += /usr/bin
    basket_desktop.files += tools/basketpwd.desktop
    basket_desktop.path += /usr/share/applications
    CopyIcons()
    basket_icon.extra = $${ICON_EXTRA}
    basket_icon.path  = $${ICON_PATH}
    basket_svg.files += images/icons/basketpwd.svg
    basket_svg.path += /usr/share/icons/hicolor/scalable/apps

    INSTALLS += target \
        basket_desktop \
        basket_icon \
        basket_svg
}
mac {
    TARGET = "Basket of passwords"
}
VERSION = 0.4.5
VERSTR = '\\"$${VERSION}\\"'
DEFINES += VER=\"$${VERSTR}\"
DEFINES += QMAKE_SET=1
OTHER_FILES += ChangeLog.txt
TRANSLATIONS += basketpwd_en.ts
