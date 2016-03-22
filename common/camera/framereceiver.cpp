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

#include "framereceiver.h"
#include "utils.h"
#include "framereceiverworker.h"

#include <QDebug>
#include <QTimerEvent>

FrameReceiver::FrameReceiver(QObject * parent)
    : QObject(parent)
    , m_thread(nullptr)
    , m_worker(nullptr)
{
    m_thread = new QThread;
    m_thread->setObjectName(QStringLiteral("FrameReceiverThread"));

    m_worker = new FrameReceiverWorker(*this);
    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    m_thread->start();
}

FrameReceiver::~FrameReceiver()
{
    m_thread->quit();
    m_thread->wait();
}

void FrameReceiver::setProcessAll(bool processAll)
{
    m_worker->setProcessAll(processAll);
}

void FrameReceiver::connectToCapture(Capture & capture)
{
    connect(&capture, &Capture::frameReady, m_worker, &FrameReceiverWorker::newFrame);
}

void FrameReceiver::processFrame(cv::Mat & mat, qint64 timestamp)
{
    Q_UNUSED(mat);
    Q_UNUSED(timestamp);
    // default implementation does nothing
}
