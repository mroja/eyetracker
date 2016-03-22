TEMPLATE = app

TARGET = pisak-eyetracker-tobii

QT += qml quick widgets websockets
CONFIG += c++11

SOURCES += main.cpp \
    ../common/eyetracker.cpp \
    ../common/smoother.cpp \
    ../common/ws_server.cpp \
    tobiieyetracker.cpp

RESOURCES += ../common/calibration.qrc

INCLUDEPATH += \
    ../tobii/sdk/include \
    ../common

LIBPATH += ../tobii/sdk/lib

LIBS += -ltobiigazecore -lopencv_core -lopencv_video

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ../common/eyetracker.h \
    ../common/etr_main.h \
    ../common/smoother.h \
    ../common/ws_server.h \
    tobiieyetracker.h

