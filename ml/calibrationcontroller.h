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

#ifndef CALIBRATIONCONTROLLER_H
#define CALIBRATIONCONTROLLER_H

#include <QObject>

#include "../common/hpe/hpe.h"
#include "datainterpolator.h"

class CalibrationController : public QObject
{
    Q_OBJECT

public:
    explicit CalibrationController(QObject * parent = 0);
    ~CalibrationController();

    void saveCalibrationData(QTextStream & out);

signals:
    void calibrationFinished();

public slots:
    void start();
    void stop();

    void updateCursorPosition(QPointF pos, qint64 timestamp);

    void updatePupilPosition(bool ok, double x, double y, double pupil_size, qint64 timestamp);

    void updateHpeData(HpeData data, qint64 timestamp);

private:
    void checkIfEnoughData();

private:
    bool m_running;

    CalibrationDataInterpolator m_interpolator;
};

#endif // CALIBRATIONCONTROLLER_H
