#-------------------------------------------------
#
# Project created by QtCreator 2016-07-11T17:27:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cuuu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    statevalidator.cpp \
    errorsdialog.cpp \
    executionwindow.cpp \
    ttlineedit.cpp \
    infodialog.cpp \
    exitdialog.cpp \
    ramlineedit.cpp

HEADERS  += mainwindow.h \
    statevalidator.h \
    styles.h \
    errorsdialog.h \
    executionwindow.h \
    ttlineedit.h \
    constants.h \
    infodialog.h \
    exitdialog.h \
    ramlineedit.h

FORMS    += \
    mainwindow.ui \
    errorsdialog.ui \
    executionwindow.ui \
    infodialog.ui \
    exitdialog.ui

RESOURCES += \
    imgs.qrc
