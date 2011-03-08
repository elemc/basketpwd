TEMPLATE = app
QT = gui \
    core \
    xml
CONFIG += qt \
    warn_on \
    debug_and_release \
    windows
DESTDIR = bin
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
    model/basketmodel.h
SOURCES = src/mainwindow.cpp \
    src/main.cpp \
    src/basketutils.cpp \
    src/changepassword.cpp \
    aboutdialog.cpp \
    src/settingsdialog.cpp \
    model/basketbaseitem.cpp \
    model/basketmodel.cpp
RESOURCES += icon-images.qrc
RC_FILE = basketpwd.rc
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
    basket_icon.files += images/basketpwd.png
    basket_icon.path += /usr/share/pixmaps
    INSTALLS += target \
        basket_desktop \
        basket_icon
}
VERSION = 0.4.3
VERSTR = '\\"$${VERSION}\\"'
DEFINES += VER=\"$${VERSTR}\"
DEFINES += QMAKE_SET=1
OTHER_FILES += ChangeLog.txt
TRANSLATIONS += basketpwd_en.ts
