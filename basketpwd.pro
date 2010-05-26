TEMPLATE = app
QT = gui \
    core \
    xml
CONFIG += qt \
    warn_on \
    debug_and_release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui \
    ui/editPwd.ui \
    ui/changepassword.ui \
    ui/editfolderdialog.ui \
    aboutdialog.ui \
    ui/settingsdialog.ui
HEADERS = src/mainwindow.h \
    src/editpwddialog.h \
    src/basketutils.h \
    src/changepassword.h \
    src/editfolderdialog.h \
    aboutdialog.h \
    src/settingsdialog.h
SOURCES = src/mainwindow.cpp \
    src/main.cpp \
    src/editpwddialog.cpp \
    src/basketutils.cpp \
    src/changepassword.cpp \
    src/editfolderdialog.cpp \
    aboutdialog.cpp \
    src/settingsdialog.cpp
RESOURCES += icon-images.qrc
RC_FILE = basketpwd.rc
LIBS += -lgcrypt
win32 { 
    LIBS += -L"d:\develop\tools\gcrypt\lib" -lgpg-error -lmingw32
    INCLUDEPATH += "d:\develop\tools\gcrypt\include" \
                    "d:\develop\tools\mingw\include"
    CONFIG += static
}
unix { 
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
VERSION = 0.3.5
OTHER_FILES += ChangeLog.txt
TRANSLATIONS += basketpwd_en.ts
