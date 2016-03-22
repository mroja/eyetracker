
#include "capturethread.h"

#include <QDebug>
#include <QDateTime>
#include <opencv2/opencv.hpp>
#include "capture.h"

CaptureThread::CaptureThread(Capture & parent, int cameraIndex)
    : QThread()
    , m_parent(parent)
    , m_cameraIndex(cameraIndex)
{
    setObjectName(QStringLiteral("CaptureThread%1").arg(m_cameraIndex));
}

void CaptureThread::run()
{
    const QThread * const thread = QThread::currentThread();

    while(true)
    {
        if(thread->isInterruptionRequested())
            break;

        cv::VideoCapture cap(m_cameraIndex);

        if(!cap.isOpened())
        {
            QThread::msleep(500);
            continue;
        }

        while(true)
        {
            if(thread->isInterruptionRequested())
                break;

            cv::Mat frame;

            // Blocks until a new frame is ready
            if(!cap.read(frame))
            {
                qDebug() << QStringLiteral("read frame failed for camera %1").arg(m_cameraIndex);
                break;
            }

            emit m_parent.frameReady(frame, QDateTime::currentMSecsSinceEpoch());
        }

        if(thread->isInterruptionRequested())
            break;

        QThread::msleep(500);
    }
}
