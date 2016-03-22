/*
 * This file is part of PISAK project.
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
 */

#include <cstring>
#include <iostream>

#include <QApplication>
#include <QtQml>

#include "ws_server.h"

template<typename EyeTrackerType>
int etr_main(int argc, char * argv[])
{
    // make sure floats are printed with dot regardless of system locale
    std::setlocale(LC_ALL, "C");
    std::locale::global(std::locale("C"));
    std::cout.imbue(std::locale("C"));
    std::cin.imbue(std::locale("C"));

    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Eyetracker Server");

    QCommandLineParser parser;
    parser.setApplicationDescription("Eyetracker Server");
    parser.addHelpOption();

    parser.addOptions({
        { {"c", "calibration"},
          QCoreApplication::translate("main", "Set calibration mode.") },
        { {"v", "verbose"},
          QCoreApplication::translate("main", "Print coordinates to console (only in tracking mode).") },
        { {"w", "ws-server"},
          QCoreApplication::translate("main", "Run websocket server (only in tracking mode).") },
        { "host",
          QCoreApplication::translate("main", "Websocket server host (default 127.0.0.1)."),
          "host", "127.0.0.1" },
        { "port",
          QCoreApplication::translate("main", "Websocket server port (default 7777)."),
          "port", "7777" }
    });

    parser.process(app);

    const bool calibrationMode = parser.isSet("calibration");
    const bool enableWebsocketServer = parser.isSet("ws-server");
    const bool verboseMode = parser.isSet("verbose");
    const QHostAddress wsHost(parser.value("host"));
    const quint16 wsPort = parser.value("port").toUInt();

    if(!calibrationMode)
    {
        EyeTrackerType tracker;
        EyetrackerWebsocketServer wsServer;

        wsServer.setDebug(verboseMode);

        if(enableWebsocketServer)
        {
            wsServer.listen(wsHost, wsPort);
        }

        QObject::connect(&tracker, &EyeTrackerType::initialized,
            [&tracker](bool success, QString errorMessage)
            {
                if(!success)
                {
                    std::cerr << "tracker initialization failed: "
                              << errorMessage.toLocal8Bit().data()
                              << std::endl;

                    QApplication::instance()->exit(1);
                }
                else
                {
                    std::cout << "tracker initialized" << std::endl;
                    tracker.startTracking();
                }
            }
        );

        QObject::connect(&tracker, &EyeTrackerType::shutdownCompleted,
            [](bool success, QString errorMessage)
            {
                if(success)
                    std::cout << "tracker shutdown completed" << std::endl;
                else
                    std::cout << "tracker shutdown error: "
                              << errorMessage.toLocal8Bit().data()
                              << std::endl;
            }
        );

        QObject::connect(&tracker, &EyeTrackerType::gazeData,
            [&wsServer, verboseMode](bool eyeDetected, QPointF pt, double timestamp)
            {
                if(verboseMode)
                {
                    std::cout << std::fixed;
                    std::cout.precision(3);
                    std::cout << "gaze_pos: " << pt.x() << " " << pt.y();
                    std::cout.precision(6);
                    std::cout << ", timestamp: " << timestamp;
                    std::cout << std::defaultfloat << std::endl;
                }

                const QJsonDocument jsonDoc(QJsonObject{
                    { "x", pt.x() },
                    { "y", pt.y() },
                    { "timestamp", timestamp },
                    { "eye_detected", eyeDetected }
                });

                // qDebug() << QString(jsonDoc.toJson(QJsonDocument::Compact));

                wsServer.sendMessageToAllClients(jsonDoc.toJson(QJsonDocument::Compact));
            }
        );

        if(tracker.loadConfig())
            std::cout << "configuration loaded" << std::endl;
        else
            std::cout << "error loading configuration" << std::endl;

        tracker.initialize();

        return app.exec();
    }
    else // calibration mode
    {
        qmlRegisterType<EyeTrackerType>("pisak.eyetracker", 1, 0, "Eyetracker");

        QQmlApplicationEngine engine;
        engine.load(QUrl(QStringLiteral("qrc:///calibration.qml")));

        return app.exec();
    }
}
