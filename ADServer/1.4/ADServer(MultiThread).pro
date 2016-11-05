#-------------------------------------------------
#
# Project created by QtCreator 2016-09-03T15:07:39
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ADServer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    file.cpp \
    node.cpp \
    parsexml.cpp \
    xmleditdialog.cpp \
    tcpserver.cpp \
    thread.cpp

HEADERS  += mainwindow.h \
    file.h \
    node.h \
    parsexml.h \
    xmleditdialog.h \
    tcpserver.h \
    thread.h

FORMS    += mainwindow.ui \
    xmleditdialog.ui

TRANSLATIONS += ts_en_us.ts

CODECFORTR = UTF-8

RESOURCES += \
    adres.qrc

RC_FILE = icon.rc

#QTPLUGIN += qcncodecs
