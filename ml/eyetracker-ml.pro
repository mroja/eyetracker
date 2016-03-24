TEMPLATE = app

TARGET = eyetracker-ml

QT += widgets opengl websockets

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

SOURCES += main.cpp \
    calibrationcontroller.cpp \
    datainterpolator.cpp \
    calibrationwindow.cpp \
    ../common/hpe/glm.cpp \
    ../common/hpe/hpe.cpp \
    ../common/camera/pupildetector.cpp \
    ../common/camera/utils.cpp \
    ../common/camera/pupildetectorsetupdialog.cpp \
    ../common/camera/imageviewer.cpp \
    ../common/camera/framereceiver.cpp \
    ../common/camera/capturethread.cpp \
    ../common/camera/capture.cpp \
    ../common/camera/framereceiverworker.cpp \
    tracker.cpp

HEADERS += \
    calibrationcontroller.h \
    datainterpolator.h \
    calibrationwindow.h \
    ../common/pstream.h \
    ../common/hpe/glm.h \
    ../common/hpe/hpe.h \
    ../common/camera/pupildetector.h \
    ../common/camera/utils.h \
    ../common/camera/pupildetectorsetupdialog.h \
    ../common/camera/imageviewer.h \
    ../common/camera/framereceiver.h \
    ../common/camera/capturethread.h \
    ../common/camera/capture.h \
    ../common/camera/framereceiverworker.h \
    tracker.h

FORMS += ../common/camera/camera_setup.ui

RESOURCES +=

# Default rules for deployment.
include(deployment.pri)
