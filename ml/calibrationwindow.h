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

#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPen>
#include <QBrush>

#include <QGraphicsView>
#include <QGraphicsEllipseItem>

/*
class CalibrationWindow : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CalibrationWindow(QWidget * parent = 0);

signals:
    void cursorPos(QPointF pos, qint64 timestamp);

public slots:
    void startMovement();
    void startEmittingData();

    void stopMovement();
    void stopEmittingData();

    void stopAll();

protected:
    void resizeEvent(QResizeEvent * event) override;
    void timerEvent(QTimerEvent * event) override;

private:
    void recalculateSceneParams();
    void updatePosition();

    QGraphicsScene m_scene;
    QGraphicsEllipseItem * m_ellipse;

private:
    bool m_running;
    bool m_emittingData;

    int m_timer;
    QElapsedTimer m_elapsed_timer;

private:
    qreal m_t;
    qreal m_speed;

    qreal m_margin_x;
    qreal m_margin_y;
    qreal m_offset_x;
    qreal m_offset_y;
    qreal m_ampl_x;
    qreal m_ampl_y;
    qreal m_coeff_x;
    qreal m_coeff_y;
    qreal m_coeff_delta;
};
*/



class CalibrationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationWindow(QWidget * parent = 0);

signals:
    void cursorPos(QPointF pos, qint64 timestamp);

public slots:
    void startMovement();
    void startEmittingData();

    void stopMovement();
    void stopEmittingData();

    void stopAll();

protected:
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent * event) override;
    void timerEvent(QTimerEvent * event) override;

private:
    bool m_running;
    bool m_emittingData;

    int m_timer;
    QElapsedTimer m_elapsed_timer;

    qreal m_t;
    qreal m_speed;

    qreal m_margin_x;
    qreal m_margin_y;
    qreal m_offset_x;
    qreal m_offset_y;
    qreal m_ampl_x;
    qreal m_ampl_y;
    qreal m_coeff_x;
    qreal m_coeff_y;
    qreal m_coeff_delta;

    QPen m_pen;
    QBrush m_brush;
    qreal m_circle_size;
};

#endif // CALIBRATIONWIDGET_H
