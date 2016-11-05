#-------------------------------------------------
#
# Project created by QtCreator 2016-09-06T21:11:10
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ADClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    parsexml.cpp \
    node.cpp \
    filerecv.cpp

HEADERS  += mainwindow.h \
    parsexml.h \
    node.h \
    filerecv.h

FORMS    += mainwindow.ui

QTPLUGIN += qcncodecs
