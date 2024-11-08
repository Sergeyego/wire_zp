#-------------------------------------------------
#
# Project created by QtCreator 2013-08-26T10:20:36
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wire_zp
TEMPLATE = app

include(xlsx/qtxlsx.pri)


SOURCES += main.cpp\
    db/dbcombobox.cpp \
    db/dbdateedit.cpp \
    db/dbrelationeditdialog.cpp \
    db/executor.cpp \
    db/tablemodel.cpp \
    doublelineedit.cpp \
    formjob.cpp \
    formliter.cpp \
    formrab.cpp \
    formrepnorm.cpp \
    httpsyncmanager.cpp \
    mainwindow.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    db/dbxlsx.cpp \
    modelro.cpp \
    modelzon.cpp \
    progressexecutor.cpp \
    progressreportdialog.cpp \
    rels.cpp \
    tableview.cpp \
    truncatepremdialog.cpp

HEADERS  += mainwindow.h \
    db/dbcombobox.h \
    db/dbdateedit.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbrelationeditdialog.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    db/dbxlsx.h \
    db/executor.h \
    db/tablemodel.h \
    doublelineedit.h \
    formjob.h \
    formliter.h \
    formrab.h \
    formrepnorm.h \
    httpsyncmanager.h \
    modelro.h \
    modelzon.h \
    progressexecutor.h \
    progressreportdialog.h \
    rels.h \
    tableview.h \
    truncatepremdialog.h

FORMS    += mainwindow.ui \
    db/dblogin.ui \
    db/dbrelationeditdialog.ui \
    formjob.ui \
    formliter.ui \
    formrab.ui \
    formrepnorm.ui \
    progressreportdialog.ui \
    truncatepremdialog.ui

RESOURCES += \
    res.qrc
RC_FILE = ico.rc

DISTFILES += \
    ico.rc
