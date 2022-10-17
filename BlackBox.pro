QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlackBox
TEMPLATE = app

SOURCES += \
    dsp.cpp \
    dspengine.cpp \
    dspsettings.cpp \
    main.cpp \
    blackbox.cpp

HEADERS += \
    blackbox.h \
    dsp.h \
    dspengine.h \
    dspsettings.h \
    qringbuffer.h

FORMS += \
    blackbox.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../LIBS/portaudio/lib/release/ -lportaudio_x86
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../LIBS/portaudio/lib/debug/ -lportaudio_x86

INCLUDEPATH += $$PWD/../../LIBS/portaudio/include
DEPENDPATH += $$PWD/../../LIBS/portaudio/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../LIBS/lame-3.100/lib/release/ -llibmp3lame
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../LIBS/lame-3.100/lib/debug/ -llibmp3lame

INCLUDEPATH += $$PWD/../../LIBS/lame-3.100/include
DEPENDPATH += $$PWD/../../LIBS/lame-3.100/include
