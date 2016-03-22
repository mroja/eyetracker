/*
 * This file is part of PISAK.
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

#include "calibrationcontroller.h"

#include <QDir>
#include <QFile>
#include <QDateTime>

CalibrationController::CalibrationController(QObject * parent)
        : QObject(parent)
        , m_running(false)
{
}

CalibrationController::~CalibrationController()
{
}

void CalibrationController::start()
{
    m_interpolator.clear();
    m_running = true;
}

void CalibrationController::stop()
{
    m_running = false;
}

void CalibrationController::updateCursorPosition(QPointF pos, qint64 timestamp)
{
    if(m_running)
    {
        m_interpolator.appendCursorData(pos, timestamp);
        checkIfEnoughData();
    }
}

void CalibrationController::updatePupilPosition(bool ok, double x, double y, double pupilSize, qint64 timestamp)
{
    Q_UNUSED(pupilSize);

    if(m_running && ok)
    {
        m_interpolator.appendPupilData(QPointF(x, y), timestamp);
        checkIfEnoughData();
    }
}

void CalibrationController::updateHpeData(HpeData data, qint64 timestamp)
{
    if(m_running)
    {
        m_interpolator.appendHpeData(data, timestamp);
        checkIfEnoughData();
    }
}

void CalibrationController::checkIfEnoughData()
{
    const std::array<std::size_t, 3> counts = m_interpolator.getDataCounts();

    const std::size_t countThreshold = 2000;

    bool gotEnoughData = true;

    if(counts.size() > 0)
    {
        for(std::size_t i = 0; i < counts.size(); i++)
        {
            if(counts[i] < countThreshold)
            {
                gotEnoughData = false;
                break;
            }
        }
    }
    else
    {
        gotEnoughData = false;
    }

    if(gotEnoughData)
    {
        m_running = false;

        m_interpolator.interpolate();

        QString fileName = QDir(QDir::homePath()).
                filePath(QStringLiteral("pisak/eyetracker/ml/etr-data-%1.json").
                         arg(QDateTime::currentMSecsSinceEpoch()));


        QFile dataFile(fileName);
        if(dataFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            QTextStream out(&dataFile);
            m_interpolator.saveCalibrationData(out);
            std::wcout << L"calibration data written to: " << fileName.toStdWString() << std::endl;
        }
        else
        {
            std::wcout << L"could not open " << fileName.toStdWString() << "for writing" << std::endl;
        }

        emit calibrationFinished();
    }
}
