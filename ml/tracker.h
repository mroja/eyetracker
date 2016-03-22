#ifndef TRACKER_H
#define TRACKER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "../common/hpe/hpe.h"

class Tracker;

class PredictorThread : public QThread
{
    Q_OBJECT

public:
    explicit PredictorThread(Tracker & tracker);

protected:
    void run() override;

private:
    Tracker & m_tracker;
};


class Tracker : public QObject
{
    Q_OBJECT
public:
    explicit Tracker(QObject * parent = 0);
    ~Tracker();

    void start();
    void stop();

    friend class PredictorThread;

public slots:
    void updatePupilPosition(bool ok, double x, double y, double pupil_size, qint64 timestamp);
    void updateHpeData(HpeData data, qint64 timestamp);

private:
    PredictorThread m_thread;

    bool m_running;

    QMutex m_dataMutex;
    bool m_newDataReady;

    QPointF m_pupilData;
    HpeData m_hpeData;

    bool m_pupilDataIsEmpty;
    bool m_hpeDataIsEmpty;
};

#endif // TRACKER_H
