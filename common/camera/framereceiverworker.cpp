#include "framereceiverworker.h"
#include "framereceiver.h"
#include "utils.h"

#include <QDebug>
#include <QPixmap>
#include <QTimerEvent>

FrameReceiverWorker::FrameReceiverWorker(FrameReceiver & parent)
    : QObject()
    , m_parent(parent)
    , m_timestamp(0)
    , m_processAll(false)
{
}

void FrameReceiverWorker::setProcessAll(bool processAll)
{
    m_processAll = processAll;
}

void FrameReceiverWorker::newFrame(const cv::Mat & frame, qint64 timestamp)
{
    if(m_processAll)
        process(frame, timestamp);
    else
        queue(frame, timestamp);
}

void FrameReceiverWorker::queue(const cv::Mat & frame, qint64 timestamp)
{
    if(!m_frame.empty())
        qDebug() << "Converter dropped frame!";
    m_frame = frame;
    m_timestamp = timestamp;
    if(!m_timer.isActive())
        m_timer.start(0, this);
}

void FrameReceiverWorker::process(cv::Mat frame, qint64 timestamp)
{
    m_parent.processFrame(frame, timestamp);
    emit m_parent.imageReady(QPixmap::fromImage(convertMatToQImage(frame)));
}

void FrameReceiverWorker::timerEvent(QTimerEvent * ev)
{
    if(ev->timerId() != m_timer.timerId())
        return;

    process(m_frame, m_timestamp);
    m_frame.release();
    m_timestamp = 0;
    m_timer.stop();
}
