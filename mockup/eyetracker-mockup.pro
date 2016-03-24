TEMPLATE = app

TARGET = eyetracker-mockup

QT += qml quick widgets websockets
CONFIG += c++11

LIBS += -lopencv_core -lopencv_video

SOURCES += main.cpp \
    ../common/eyetracker.cpp \
    ../common/smoother.cpp \
    ../common/ws_server.cpp \
    mockupeyetracker.cpp

RESOURCES += ../common/calibration.qrc

INCLUDEPATH += ../common

HEADERS += \
    ../common/eyetracker.h \
    ../common/etr_main.h \
    ../common/smoother.h \
    ../common/ws_server.h \
    mockupeyetracker.h

