#include "tracker.h"

#include <QDir>
#include <QDebug>

#include <boost/algorithm/string/predicate.hpp>

PredictorThread::PredictorThread(Tracker & tracker)
    : QThread()
    , m_tracker(tracker)
{
}

void PredictorThread::run()
{
    QString scriptPath(QDir(QDir::homePath()).filePath(QStringLiteral("pisak/eyetracker/ml/python/predict.py")));

    QDir configDir(QDir(QDir::homePath()).filePath(QStringLiteral(".pisak/eyetracker/ml")));
    if(!configDir.exists())
        configDir.mkpath(".");

    QString configFile(configDir.filePath(QStringLiteral("predictors.json")));

    QString cmd = QStringLiteral("python3 \"%1\" \"%2\"").arg(scriptPath).arg(configFile);

    auto buf = cmd.toLocal8Bit();
    std::cout << "predictor cmd: " << buf.data() << std::endl;

    redi::pstream tracker_process(cmd.toLocal8Bit().data(),
                                  redi::pstreams::pstdout | redi::pstreams::pstderr);

    if(!tracker_process.is_open())
        std::cerr << "Creating tracker process failed!" << std::endl;
    else
        std::cout << "Tracker process created!" << std::endl;

    // read and ignore first line of output
    /*{
        std::string tmp;
        tracker_process >> tmp;
        std::cout << "hpe tracker output: " << tmp << std::endl;
    }*/

    QPointF pupilData;
    HpeData hpeData;

    //double x;
    //double y;
    std::stringstream s;
    std::string line;

    while(true)
    {
        QThread::msleep(10);

        if(isInterruptionRequested())
            return;

        // lock scope
        {
            QMutexLocker locker(&m_tracker.m_dataMutex);

            if(m_tracker.m_newDataReady)
            {
                pupilData = m_tracker.m_pupilData;
                hpeData = m_tracker.m_hpeData;
            }
            else // if(!m_tracker.m_newDataReady)
            {
                continue;
            }
        }

        s.str();
        s << pupilData.x() << ';' << pupilData.y() << ';'
          << hpeData.corners[0].x() << ';' << hpeData.corners[0].y() << ';'
          << hpeData.corners[1].x() << ';' << hpeData.corners[1].y() << ';'
          << hpeData.corners[2].x() << ';' << hpeData.corners[2].y() << ';'
          << hpeData.corners[3].x() << ';' << hpeData.corners[3].y() << ';'
          << hpeData.translation[0] << ';'
          << hpeData.translation[1] << ';'
          << hpeData.translation[2] << ';'
          << hpeData.rotation[0] << ';'
          << hpeData.rotation[1] << ';'
          << hpeData.rotation[2];

        tracker_process << s.str() << std::endl;
        tracker_process >> line;

        if(boost::starts_with(line, "gaze_pos:"))
            std::cout << line << std::endl;
    }
}

Tracker::Tracker(QObject * parent)
    : QObject(parent)
    , m_thread(*this)
    , m_running(false)
    , m_newDataReady(false)
    , m_pupilDataIsEmpty(true)
    , m_hpeDataIsEmpty(true)
{
    m_thread.start();
}

Tracker::~Tracker()
{
    m_thread.requestInterruption();
    if(!m_thread.wait(500))
        m_thread.terminate();
}

void Tracker::start()
{
    m_running = true;

    QMutexLocker locker(&m_dataMutex);
    m_newDataReady = false;
    m_pupilDataIsEmpty = true;
    m_hpeDataIsEmpty = true;
}

void Tracker::stop()
{
    m_running = false;

    QMutexLocker locker(&m_dataMutex);
    m_newDataReady = false;
    m_pupilDataIsEmpty = true;
    m_hpeDataIsEmpty = true;
}

void Tracker::updatePupilPosition(bool ok, double x, double y, double pupilSize, qint64 timestamp)
{
    Q_UNUSED(pupilSize);
    Q_UNUSED(timestamp);

    if(m_running && ok)
    {
        QMutexLocker locker(&m_dataMutex);
        m_pupilData = QPointF(x, y);
        m_pupilDataIsEmpty = false;
        if(!m_pupilDataIsEmpty && !m_hpeDataIsEmpty)
            m_newDataReady = true;
    }
}

void Tracker::updateHpeData(HpeData data, qint64 timestamp)
{
    Q_UNUSED(timestamp);

    if(m_running)
    {
        QMutexLocker locker(&m_dataMutex);
        m_hpeData = data;
        m_hpeDataIsEmpty = false;
        if(!m_pupilDataIsEmpty && !m_hpeDataIsEmpty)
            m_newDataReady = true;
    }
}
