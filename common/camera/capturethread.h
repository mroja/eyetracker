#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>

class Capture;

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    explicit CaptureThread(Capture & parent, int cameraIndex);

protected:
    void run() override;

private:
    Capture & m_parent;
    int m_cameraIndex;
};

#endif // CAPTURETHREAD_H
