QT       += core gui
QT       += multimedia
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32: QMAKE_CXXFLAGS += -execution-charset:utf-8

# make the application have authority of running on Ubuntu, and I do not know why it works
unix: QMAKE_LFLAGS += -no-pie

# Qt 5.14 VERSION can only contains numbers, no any others type of character please
# please do not put 0 before any version number, because this will cause a warnning on Qt 5.14
win32:  VERSION = 21.5.9.1140                # major.minor.patch.build
else:   VERSION = 21.5.9                    # major.minor.patch

QMAKE_TARGET_COPYRIGHT = mtr company Co., Ltd

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += E:\project\ffmpeg\install\include
DESTDIR = ../bin

SOURCES += \
    Player/qtavplayer.cpp \
    Public/appsignal.cpp \
    Public/button.cpp \
    Public/card.cpp \
    Public/widgetmediacontrol.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Player/qtavplayer.h \
    Public/appsignal.h \
    Public/button.h \
    Public/card.h \
    Public/threadsafequeue.h \
    Public/widgetmediacontrol.h \
    mainwindow.h

FORMS += \
    Public/widgetmediacontrol.ui \
    mainwindow.ui

TRANSLATIONS += \
    Translate_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = ./Resourse/icon/icon.ico

RESOURCES += \
    res.qrc

#0x0800代表和系统当前语言一致
RC_LANG = 0x0800

LIBS += -LE:\project\ffmpeg\install\bin \
-lavformat \
-lavcodec \
-lavdevice \
-lavfilter \
-lavutil \
-lswresample \
-lswscale \
