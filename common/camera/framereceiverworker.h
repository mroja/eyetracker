#ifndef FRAMERECEIVERWORKER_H
#define FRAMERECEIVERWORKER_H

#include <QObject>
#include <QBasicTimer>
#include <opencv2/core/core.hpp>

class FrameReceiver;

class FrameReceiverWorker : public QObject
{
    Q_OBJECT

public:
    explicit FrameReceiverWorker(FrameReceiver & parent);

    void setProcessAll(bool processAll);

signals:
    void imageReady(const QImage & image);

public slots:
    void newFrame(const cv::Mat & frame, qint64 timestamp);

private:
    void queue(const cv::Mat & frame, qint64 timestamp);
    void process(cv::Mat frame, qint64 timestamp);
    void timerEvent(QTimerEvent * ev);

private:
    FrameReceiver & m_parent;

    QBasicTimer m_timer;
    cv::Mat m_frame;
    qint64 m_timestamp;
    bool m_processAll;
};

#endif // FRAMERECEIVERWORKER_H
