#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T21:32:23
#
#-------------------------------------------------

QT       += xml

TARGET = basketpwd
TEMPLATE = lib

DEFINES += BASKETPWD_LIBRARY

SOURCES += basketpwdlibs.cpp \
    coreplugin.cpp \
    basketutils.cpp \
    basketbaseitem.cpp

HEADERS += basketpwdlibs.h\
        basketpwd-libs_global.h \
    plugin_share.h \
    coreplugin.h \
    basketutils.h \
    basketbaseitem.h

unix {
    LIBS += -lcrypto -lssl
}
win32 {
    CONFIG += static
    INCLUDEPATH += d:/devel/utils/openssl/openssl-shared-64/include
    LIBS += -ld:/devel/utils/openssl/openssl-shared-64/lib/libeay32
}

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE426FAEC
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = basketpwd.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
