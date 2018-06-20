QT -= gui
QT += multimedia

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

linux {
    contains(QT_ARCH, x86_64){
        LIBS += \
            -L../../lib/linux/x86_64
    } else {
        LIBS += \
            -L../../lib/linux/i386
    }
}

macx {
    contains(QT_ARCH, x86_64){
        LIBS += \
            -L../../lib/mac/x86_64
    } else {
        LIBS += \
            -L../../lib/mac/i386
    }
}

win32 {
    contains(QT_ARCH, x86_64){
        LIBS += \
            -L../../lib/windows/amd64
    } else {
        LIBS += \
            -L../../lib/windows/i686
    }
}

INCLUDEPATH += ../../include
LIBS += -lpv_porcupine
