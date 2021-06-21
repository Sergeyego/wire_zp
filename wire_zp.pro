#-------------------------------------------------
#
# Project created by QtCreator 2013-08-26T10:20:36
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wire_zp
TEMPLATE = app

include(xlsx/qtxlsx.pri)


SOURCES += main.cpp\
        mainwindow.cpp \
    models.cpp \
    modeljob.cpp \
    zpsqlmodel.cpp \
    zpdialog.cpp \
    truncatepremdialog.cpp \
    premdialog.cpp \
    jobtypedialog.cpp \
    edttndialog.cpp \
    chtypjobsqlmodel.cpp \
    edtrabdialog.cpp \
    modelchk.cpp \
    tndialog.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    db/dbxlsx.cpp \
    formjob.cpp \
    tabwidget.cpp \
    formrepnorm.cpp \
    jobsqlmodel.cpp \
    zonwidget.cpp

HEADERS  += mainwindow.h \
    models.h \
    modeljob.h \
    zpsqlmodel.h \
    zpdialog.h \
    truncatepremdialog.h \
    premdialog.h \
    jobtypedialog.h \
    edttndialog.h \
    chtypjobsqlmodel.h \
    edtrabdialog.h \
    modelchk.h \
    tndialog.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    db/dbxlsx.h \
    formjob.h \
    tabwidget.h \
    formrepnorm.h \
    jobsqlmodel.h \
    zonwidget.h

FORMS    += mainwindow.ui \
    zpdialog.ui \
    truncatepremdialog.ui \
    premdialog.ui \
    jobtypedialog.ui \
    edttndialog.ui \
    edtrabdialog.ui \
    tndialog.ui \
    db/dblogin.ui \
    formjob.ui \
    formrepnorm.ui

RESOURCES += \
    res.qrc
RC_FILE = ico.rc

DISTFILES += \
    ico.rc
