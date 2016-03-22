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
 *
 * Copyright 2015, Alex Khrabrov <alex@mroja.net>
 *
 */

#ifndef CAPTURE_H
#define CAPTURE_H

#include <QObject>
#include <opencv2/core/core.hpp>

Q_DECLARE_METATYPE(cv::Mat)

class CaptureThread;

class Capture : public QObject
{
    Q_OBJECT

public:
    explicit Capture(QObject * parent = 0);
    ~Capture();

public slots:
    void start(int cameraIndex = 0);
    void stop();

signals:
    void frameReady(const cv::Mat & frame, qint64 timestamp);

private:
    CaptureThread * m_thread;
};

#endif // CAPTURE_H
