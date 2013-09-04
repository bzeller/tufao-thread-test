#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T00:23:11
#
#-------------------------------------------------

QT       += core
QT       -= gui
CONFIG   += C++11 TUFAO1

TARGET = ThreadTestHandler
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../exec

SOURCES += threadtesthandler.cpp \
    threadtesthandlerfactory.cpp

HEADERS += threadtesthandler.h \
    threadtesthandlerfactory.h

OTHER_FILES += ThreadTestHandler.json

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
