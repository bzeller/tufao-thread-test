#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T10:39:43
#
#-------------------------------------------------

QT       += core
QT       -= gui
CONFIG   += C++11 TUFAO1

TARGET = NotFoundHandler
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../exec


SOURCES += notfoundhandler.cpp

HEADERS += notfoundhandler.h
OTHER_FILES += NotFoundHandler.json

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
