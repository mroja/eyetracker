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

#ifndef FRAMERECEIVER_H
#define FRAMERECEIVER_H

#include <QThread>

#include "capture.h"

class FrameReceiverWorker;

class FrameReceiver : public QObject
{
    Q_OBJECT

public:
    explicit FrameReceiver(QObject * parent = 0);
    ~FrameReceiver();

    void setProcessAll(bool processAll = true);
    void connectToCapture(Capture & capture);

protected:
    virtual void processFrame(cv::Mat & mat, qint64 timestamp);

signals:
    void imageReady(const QPixmap &);

private:
    QThread * m_thread;
    FrameReceiverWorker * m_worker;

    friend class FrameReceiverWorker;
};

#endif // FRAMERECEIVER_H
