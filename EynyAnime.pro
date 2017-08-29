#-------------------------------------------------
#
# Project created by QtCreator 2017-05-02T04:45:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EynyAnime
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    eynywebcontrol.cpp \
    zippysharewebcontrol.cpp \
    animelistwidget.cpp \
    animetreewidget.cpp \
    mainwebwidget.cpp \
    supercontrol.cpp \
    downloadwidget.cpp \
    animeinfo.cpp \
    animedetailwidget.cpp \
    treewidget.cpp \
    myitemdelegate.cpp \
    mywidget.cpp \
    animehomepage.cpp

HEADERS  += mainwindow.h \
    eynywebcontrol.h \
    zippysharewebcontrol.h \
    animelistwidget.h \
    animetreewidget.h \
    mainwebwidget.h \
    supercontrol.h \
    downloadwidget.h \
    animeinfo.h \
    animedetailwidget.h \
    treewidget.h \
    myitemdelegate.h \
    mywidget.h \
    animehomepage.h

include("D:\All\Code\QT\module\easywebcontrol\easywebcontrol.pri")

#版本信息
VERSION = 0.0.0.1

#圖標
#RC_ICONS = Images / MyApp.ico

#公司名稱
QMAKE_TARGET_COMPANY = "Digia"

#產品名稱
QMAKE_TARGET_PRODUCT = "Qt Creator"

#文件說明
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on Qt 5.7.0 (MSVC 2013, 32 bit)"

#版權信息
QMAKE_TARGET_COPYRIGHT = "Copyright 2008-2016 The Qt Company Ltd. All rights reserved."

#中文（繁體）
RC_LANG = 0x7C04
