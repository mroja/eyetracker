/*
 * This file is part of PISAK.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2015, Alex Khrabrov <alex@mroja.net>
 *
 */

#include <QApplication>

#include "../common/camera/pupildetector.h"
#include "../common/camera/pupildetectorsetupdialog.h"
#include "../common/hpe/hpe.h"

#include "calibrationcontroller.h"
#include "calibrationwindow.h"
#include "tracker.h"

int main(int argc, char * argv[])
{
    qRegisterMetaType<cv::Mat>();
    qRegisterMetaType<HpeData>();

    // make sure floats are printed with dot regardless of system locale
    std::setlocale(LC_ALL, "C");
    std::locale::global(std::locale("C"));
    std::cout.imbue(std::locale("C"));
    std::cin.imbue(std::locale("C"));

    bool trackingOnly = false;

    if(argc > 1)
    {
        for(int i = 1; i < argc; i++)
        {
            if(std::strcmp(argv[i], "--tracking") == 0)
            {
                trackingOnly = true;
                break;
            }
        }
    }

    QApplication app(argc, argv);

    int pupilCameraIndex = 0;
    int hpeCameraIndex = 1;
    int hpeMarkerId = 0;

    Capture capture;
    PupilDetectingFrameReceiver pupilDetector;
    pupilDetector.setProcessAll(false);
    pupilDetector.connectToCapture(capture);

    capture.start(pupilCameraIndex);

    HpeWidget hpe(hpeCameraIndex, hpeMarkerId);

    // start hpe
    hpe.show();

    if(trackingOnly)
    {
        Tracker tracker;
        tracker.start();

        QObject::connect(&pupilDetector, &PupilDetectingFrameReceiver::pupilData,
                         &tracker, &Tracker::updatePupilPosition);

        QObject::connect(&hpe, &HpeWidget::hpeData,
                         &tracker, &Tracker::updateHpeData);

        //....
    }
    else
    {
        CalibrationWindow cursorWidget;
        cursorWidget.resize(800, 600);
        cursorWidget.setWindowTitle(QStringLiteral("Eyetracker calibration"));

        PupilDetectorSetupDialog camPreview;

        CalibrationController controller;

        QObject::connect(&camPreview, &PupilDetectorSetupDialog::cameraIndexChanged,
            [&](int cameraIndex)
            {
                capture.start(cameraIndex);
            }
        );

        // connect data sources
        QObject::connect(&cursorWidget, &CalibrationWindow::cursorPos,
                         &controller, &CalibrationController::updateCursorPosition);

        QObject::connect(&pupilDetector, &PupilDetectingFrameReceiver::pupilData,
                         &controller, &CalibrationController::updatePupilPosition);

        QObject::connect(&hpe, &HpeWidget::hpeData,
                         &controller, &CalibrationController::updateHpeData);

        // show camera setup window
        camPreview.setVideoSource(&pupilDetector, 0);
        camPreview.show();

        QObject::connect(&camPreview, &PupilDetectorSetupDialog::finished,
            [&](int result)
            {
                if(result != PupilDetectorSetupDialog::Accepted)
                {
                    QCoreApplication::instance()->quit();
                    return;
                }

                //cursorWidget.show();
                cursorWidget.showFullScreen();

                QTimer::singleShot(500, [&](){
                    cursorWidget.startMovement();
                    QTimer::singleShot(500, [&](){
                        cursorWidget.startEmittingData();
                        QTimer::singleShot(500, [&](){
                            controller.start();
                        });
                    });
                });
            }
        );

        QObject::connect(&controller, &CalibrationController::calibrationFinished,
            []()
            {
                QCoreApplication::instance()->quit();
            }
        );
    }

    return app.exec();
}
