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

#include "datainterpolator.h"

bool CalibrationDataInterpolator::interpolate()
{
    // assume minimum 10 measurements
    if(m_cursorData.size() < 10 ||
       m_pupilData.size() < 10 ||
       m_hpeData.size() < 10)
    {
        qDebug() << "need more data";
        return false;
    }

    const qint64 startTimestamp =
        std::max(std::max(m_cursorData[0].timestamp,
                          m_pupilData[0].timestamp),
                 m_hpeData[0].timestamp);

    // get all timestamps
    std::set<qint64> timestamps;
    for(const auto & x : m_cursorData)
        timestamps.insert(x.timestamp);
    for(const auto & x : m_pupilData)
        timestamps.insert(x.timestamp);
    for(const auto & x : m_hpeData)
        timestamps.insert(x.timestamp);

    auto currentTimestampIterator = timestamps.begin();

    // skip to starting timestamp
    for(; *currentTimestampIterator < startTimestamp; currentTimestampIterator++);

    m_interpolatedData.clear();

    while(true)
    {
        if(currentTimestampIterator == timestamps.end())
            break;

        const qint64 currentTimestamp = *currentTimestampIterator;

        currentTimestampIterator++;

        CursorDataArray cursorData;
        PupilDataArray pupilData;
        HpeDataArray hpeData;

        const bool ok1 = interpolateAtTimestamp(m_cursorData, currentTimestamp, cursorData);
        const bool ok2 = interpolateAtTimestamp(m_pupilData, currentTimestamp, pupilData);
        const bool ok3 = interpolateAtTimestamp(m_hpeData, currentTimestamp, hpeData);

        if(!ok1 || !ok2 || !ok3)
            break;

        m_interpolatedData.emplace_back();

        InterpolatedDataArray & d = m_interpolatedData.back();

        Q_ASSERT(d.size() == cursorData.size() + pupilData.size() + hpeData.size());

        std::copy(cursorData.begin(), cursorData.end(), d.begin());
        std::copy(pupilData.begin(), pupilData.end(), d.begin() + cursorData.size());
        std::copy(hpeData.begin(), hpeData.end(), d.begin() + (cursorData.size() + pupilData.size()));
    }

    return true;
}

void CalibrationDataInterpolator::saveCalibrationData(QTextStream & out)
{
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(12);

    out << "{\n"
        << "\"calibration_data\":[";

    for(std::size_t i = 0; i < m_interpolatedData.size(); i++)
    {
        out << "[" ;

        const auto & m = m_interpolatedData[i];
        for(std::size_t j = 0; j < m.size(); j++)
        {
            out << m[j];
            if(j != m.size() - 1)
                out << ",";
        }

        out << "]";
        if(i != m_interpolatedData.size() - 1)
            out << ",\n";
        else
            out << "\n";
    }

    out << "]}";
}
