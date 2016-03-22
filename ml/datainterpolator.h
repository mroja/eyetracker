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

#ifndef DATAINTERPOLATOR_H
#define DATAINTERPOLATOR_H

#include <QDebug>
#include <QTextStream>

// for HpeData
#include "../common/hpe/hpe.h"

struct CalibrationDataInterpolator
{
public:
    typedef qint64 Timestamp;

    template<int N> using Array = std::array<double, N>;
    template<typename T> using MeasurementsArray = std::deque<T>;

    typedef Array<2> CursorDataArray;
    typedef Array<2> PupilDataArray;
    typedef Array<2*4 + 2*3> HpeDataArray; // 14 values

    // 2 - cursor position, 2 - pupil position, 2*4 + 2*3 - head pose data
    typedef Array<2+2+2*4+2*3> InterpolatedDataArray;

    template<typename DataType>
    struct DataWithTimestamp
    {
        Timestamp timestamp;
        DataType data;
    };

    typedef DataWithTimestamp<CursorDataArray> CursorDataWithTimestamp;
    typedef DataWithTimestamp<PupilDataArray> PupilDataWithTimestamp;
    typedef DataWithTimestamp<HpeDataArray> HpeDataWithTimestamp;

    typedef MeasurementsArray<CursorDataWithTimestamp> CursorMeasurements;
    typedef MeasurementsArray<PupilDataWithTimestamp> PupilMeasurements;
    typedef MeasurementsArray<HpeDataWithTimestamp> HpeMeasurements;

public:
    inline void clear()
    {
        m_cursorData.clear();
        m_pupilData.clear();
        m_hpeData.clear();
        m_interpolatedData.clear();
    }

    inline void appendCursorData(const QPointF & pos, qint64 timestamp)
    {
        qDebug() << "append cursor data:" << timestamp;

        m_cursorData.push_back(CursorDataWithTimestamp{timestamp,
                                                       CursorDataArray{pos.x(), pos.y()}});
    }

    inline void appendPupilData(const QPointF & pos, qint64 timestamp)
    {
        qDebug() << "append pupil data:" << timestamp;

        m_pupilData.push_back(PupilDataWithTimestamp{timestamp,
                                                     PupilDataArray{pos.x(), pos.y()}});
    }

    inline void appendHpeData(const HpeData & data, qint64 timestamp)
    {
        qDebug() << "append hpe data:" << timestamp;

        HpeDataArray d;
        for(std::size_t i = 0; i < 4; i++)
        {
            d[2*i + 0] = data.corners[i].x();
            d[2*i + 1] = data.corners[i].y();
        }
        d[8] = data.translation[0];
        d[9] = data.translation[1];
        d[10] = data.translation[2];
        d[11] = data.rotation[0];
        d[12] = data.rotation[1];
        d[13] = data.rotation[2];
        m_hpeData.push_back(HpeDataWithTimestamp{timestamp, d});
    }

    inline std::array<std::size_t, 3> getDataCounts()
    {
        return std::array<std::size_t, 3>{{ m_cursorData.size(),
                                            m_pupilData.size(),
                                            m_hpeData.size() }};
    }

    bool interpolate();
    void saveCalibrationData(QTextStream & out);

private:

    template <typename T>
    bool interpolateAtTimestamp(
            const MeasurementsArray<DataWithTimestamp<T>> & vec,
            Timestamp timestamp,
            T & data)
    {
        if(vec.size() < 2)
            return false;

        if(timestamp < vec[0].timestamp ||
           timestamp > vec[vec.size() - 1].timestamp)
            return false;

        // find points to interpolate between
        for(std::size_t i = 0; i < vec.size() - 1; i++)
        {
            if(timestamp == vec[i].timestamp)
            {
                data = vec[i].data;
                return true;
            }
            else if(timestamp == vec[i+1].timestamp)
            {
                data = vec[i+1].data;
                return true;
            }
            else if(timestamp > vec[i].timestamp &&
                    timestamp < vec[i+1].timestamp)
            {
                data = interpolateArray(timestamp, vec[i].timestamp, vec[i+1].timestamp, vec[i].data, vec[i+1].data);
                return true;
            }
        }

        Q_UNREACHABLE(); // should never happen

        return false;
    }

    // linearly interpolate raw data values between two timestamps
    template <std::size_t N>
    Array<N> interpolateArray(
            Timestamp t,
            Timestamp t1,
            Timestamp t2,
            const Array<N> & v1,
            const Array<N> & v2)
    {
        Q_ASSERT(t2 > t1);
        Q_ASSERT(t > t1 && t < t2);

        const double c(double(t - t2) / double(t2 - t1));
        Array<N> v;
        for(std::size_t i = 0; i < N; i++)
            v[i] = v1[i] + c * (v2[i] - v1[i]);
        return v;
    }

private:
    CursorMeasurements m_cursorData;
    PupilMeasurements m_pupilData;
    HpeMeasurements m_hpeData;

    MeasurementsArray<InterpolatedDataArray> m_interpolatedData;
};

#endif // DATAINTERPOLATOR_H
