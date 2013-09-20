#-------------------------------------------------
#
# Project created by QtCreator 2013-09-10T09:58:35
#
#-------------------------------------------------

QT       += core
QT       -= gui
CONFIG   += C++11 TUFAO1

TARGET = JsonHandler
TEMPLATE = lib
CONFIG += plugin debug


SOURCES += jsonhandler.cpp \
    jsonhandlerplugin.cpp \
    abstractjsonhandler.cpp \
    httpconnection.cpp

HEADERS += jsonhandler.h \
    jsonhandlerplugin.h \
    abstractjsonhandler.h \
    httpconnection.h
OTHER_FILES += JsonHandler.json

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
