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
    dialogtabel.cpp \
    doublelineedit.cpp \
    formcalcwage.cpp \
    formjob.cpp \
    formliter.cpp \
    formpremmon.cpp \
    formrab.cpp \
    formrepmon.cpp \
    formrepnorm.cpp \
    formreptarif.cpp \
    formtn.cpp \
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
    tndialog.cpp \
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
    dialogtabel.h \
    doublelineedit.h \
    formcalcwage.h \
    formjob.h \
    formliter.h \
    formpremmon.h \
    formrab.h \
    formrepmon.h \
    formrepnorm.h \
    formreptarif.h \
    formtn.h \
    httpsyncmanager.h \
    modelro.h \
    modelzon.h \
    progressexecutor.h \
    progressreportdialog.h \
    rels.h \
    tableview.h \
    tndialog.h \
    truncatepremdialog.h

FORMS    += mainwindow.ui \
    db/dblogin.ui \
    db/dbrelationeditdialog.ui \
    dialogtabel.ui \
    formcalcwage.ui \
    formjob.ui \
    formliter.ui \
    formpremmon.ui \
    formrab.ui \
    formrepmon.ui \
    formrepnorm.ui \
    formreptarif.ui \
    formtn.ui \
    progressreportdialog.ui \
    tndialog.ui \
    truncatepremdialog.ui

RESOURCES += \
    res.qrc
	
win32:RC_FILE = ico.rc

DISTFILES += \
	ico.rc

macx:ICON = ico.icns
