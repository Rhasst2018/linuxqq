#-------------------------------------------------
#
# Project created by QtCreator 2014-04-20T01:07:07
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qq
TEMPLATE = app


SOURCES += main.cpp\
        loginwidget.cpp \
    registerwidget.cpp \
    mainwidget.cpp \
    chatwindow.cpp \
    mytoolbutton.cpp \
    addfriendwidget.cpp \
    changepasswdwidget.cpp

HEADERS  += loginwidget.h \
    registerwidget.h \
    mainwidget.h \
    chatwindow.h \
    mytoolbutton.h \
    addfriendwidget.h \
    changepasswdwidget.h

FORMS    += loginwidget.ui \
    registerwidget.ui \
    mainwidget.ui \
    chatwindow.ui \
    addfriendwidget.ui \
    changepasswdwidget.ui

RESOURCES += \
    qqrsc.qrc

RC_FILE = qq.rc
