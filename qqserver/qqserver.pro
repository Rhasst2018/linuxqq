#-------------------------------------------------
#
# Project created by QtCreator 2014-04-25T00:09:25
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qqserver
TEMPLATE = app


SOURCES += main.cpp\
        qqtcpserver.cpp \
    mytcpserver.cpp \
    mythread.cpp


HEADERS  += qqtcpserver.h \
    mytcpserver.h \
    mythread.h \
    sqlconnection.h


FORMS    += qqtcpserver.ui
