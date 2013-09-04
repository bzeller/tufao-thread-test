#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T10:51:29
#
#-------------------------------------------------

QT       += core
QT       -= gui
CONFIG   += C++11 TUFAO1

TARGET = FileServerHandler
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../exec

SOURCES += fileserverhandler.cpp

HEADERS += fileserverhandler.h
OTHER_FILES += FileServerHandler.json

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
