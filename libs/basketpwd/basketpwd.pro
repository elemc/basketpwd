#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T21:32:23
#
#-------------------------------------------------

QT       += xml

TARGET = basketpwd
TEMPLATE = lib

DEFINES += BASKETPWD_LIBRARY

DESTDIR = ../../bin/

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
    eval($$QMAKE_HOST.arch = x86) {
       target.path = /lib
    }
    eval($$QMAKE_HOST.arch = x86_64) {
       target.path = /lib64
    }

    # Headers for include
    include_headers.files += $${HEADERS}
    include_headers.path = /include/basketpwd

    INSTALLS += target include_headers
}
win32 {
    CONFIG += static
    INCLUDEPATH += d:/devel/utils/openssl/openssl-shared-64/include
    LIBS += -ld:/devel/utils/openssl/openssl-shared-64/lib/libeay32
}
