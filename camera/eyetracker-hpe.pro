TEMPLATE = app

TARGET = eyetracker-hpe

QT += widgets qml quick websockets

CONFIG += c++11

LIBS += -lGL -lGLU \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_calib3d \
    -lopencv_video

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

INCLUDEPATH += ../common

SOURCES += main.cpp \
    cameraeyetracker.cpp \
    calibration.cpp \
    ../common/camera/capture.cpp \
    ../common/camera/pupildetector.cpp \
    ../common/camera/pupildetectorsetupdialog.cpp \
    ../common/camera/utils.cpp \
    ../common/camera/imageviewer.cpp \
    ../common/eyetracker.cpp \
    ../common/smoother.cpp \
    ../common/ws_server.cpp \
    ../common/hpe/glm.cpp \
    ../common/hpe/hpe.cpp \
    ../common/camera/capturethread.cpp \
    ../common/camera/framereceiverworker.cpp \
    ../common/camera/framereceiver.cpp

HEADERS += \
    cameraeyetracker.h \
    calibration.h \
    ../common/etr_main.h \
    ../common/eyetracker.h \
    ../common/smoother.h \
    ../common/ws_server.h \
    ../common/pstream.h \
    ../common/hpe/glm.h \
    ../common/hpe/hpe.h \
    ../common/camera/pupildetectorsetupdialog.h \
    ../common/camera/utils.h \
    ../common/camera/imageviewer.h \
    ../common/camera/capture.h \
    ../common/camera/pupildetector.h \
    ../common/camera/capturethread.h \
    ../common/camera/framereceiverworker.h \
    ../common/camera/framereceiver.h

FORMS += ../common/camera/camera_setup.ui

RESOURCES += ../common/calibration.qrc
