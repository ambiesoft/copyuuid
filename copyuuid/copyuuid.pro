#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T22:05:57
#
#-------------------------------------------------
CONFIG -= qt

# QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = copyuuid
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += copyuuid.cpp \ 
    ../../lsMisc/I18N.cpp \
    ../../lsMisc/stdwin32/stdwin32.cpp \
    ../../lsMisc/SetClipboardText.cpp \
    ../../lsMisc/UrlEncode.cpp \
    ../../lsMisc/OpenCommon.cpp \
    ../../lsMisc/UTF16toUTF8.cpp \
    ../../lsMisc/CommandLineParser.cpp

INCLUDEPATH += ../../lsMisc/stlsoft/include

HEADERS += copyuuid.h \
    ../../lsMisc/I18N.h \
    ../../lsMisc/stdwin32/stdwin32.h \
    ../../lsMisc/SetClipboardText.h \
    ../../lsMisc/UrlEncode.h \
    ../../lsMisc/OpenCommon.h \
    ../../lsMisc/UTF16toUTF8.h \
    ../../lsMisc/CommandLineParser.h

FORMS += 

RC_FILE = copyuuid.rc

DISTFILES +=

PRECOMPILED_HEADER = stdafx.h

message($$QMAKESPEC)

win32 {
    win32-g++ {
        LIBS += -lOle32 -lRpcrt4 -lshlwapi -lUser32
    }
    win32-msvc* {
        LIBS += Ole32.lib Rpcrt4.lib shlwapi.lib User32.lib
    }
}
linux-g++ {
}


# TRANSLATIONS += translations/i18n_Japanese.ts

RESOURCES += 


