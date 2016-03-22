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

#include "calibrationwindow.h"

#include <QPainter>
#include <QDateTime>
#include <QTimerEvent>
#include <QGLWidget>

/*

CalibrationWindow::CalibrationWindow(QWidget * parent)
    : QGraphicsView(parent)
    , m_running(false)
    , m_emittingData(false)
    , m_t(0.0)
    , m_speed(0.0001)
    , m_margin_x(0.0)
    , m_margin_y(0.0)
    , m_offset_x(1.0)
    , m_offset_y(1.0)
    , m_ampl_x(1.0)
    , m_ampl_y(1.0)
    , m_coeff_x(5.0)
    , m_coeff_y(4.0)
    , m_coeff_delta(0.0)
{
    resize(800, 600);

    QPen pen;
    pen.setColor(Qt::black);

    QBrush brush;
    brush.setColor(Qt::yellow);
    brush.setStyle(Qt::SolidPattern);

    m_ellipse = new QGraphicsEllipseItem(0.0, 0.0, 100.0, 100.0);
    m_ellipse->setPen(pen);
    m_ellipse->setBrush(brush);
    m_ellipse->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    m_scene.addItem(m_ellipse);

    m_scene.setBackgroundBrush(QColor("#d0d0d0"));

    recalculateSceneParams();

    setScene(&m_scene);
    setCacheMode(QGraphicsView::CacheBackground);
    setRenderHint(QPainter::Antialiasing);

    //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer)));

    // 60 fps
    m_timer = startTimer(1000 / 60, Qt::PreciseTimer);
}

void CalibrationWindow::startMovement()
{
    m_running = true;
    m_elapsed_timer.start();
}

void CalibrationWindow::startEmittingData()
{
    m_emittingData = true;
}

void CalibrationWindow::stopMovement()
{
    m_running = false;
}

void CalibrationWindow::stopEmittingData()
{
    m_emittingData = false;
}

void CalibrationWindow::stopAll()
{
    stopMovement();
    stopEmittingData();
}

void CalibrationWindow::resizeEvent(QResizeEvent * event)
{
    QGraphicsView::resizeEvent(event);
    recalculateSceneParams();
}

void CalibrationWindow::recalculateSceneParams()
{
    const qreal w = width();
    const qreal h = height();

    m_scene.setSceneRect(0, 0, contentsRect().width(), contentsRect().height());

    m_margin_x = 0.05 * w;
    m_margin_y = 0.08 * h;
    m_offset_x = 0.5 * w;
    m_offset_y = 0.5 * h;
    m_ampl_x = 0.5 * (w - 2.0 * m_margin_x);
    m_ampl_y = 0.5 * (h - 2.0 * m_margin_y);

    qreal size = (0.047 * h);
    QPointF pos = m_ellipse->rect().center();

    m_ellipse->setRect(pos.x() - size, pos.y() - size,
                       pos.x() + size, pos.y() + size);

    m_ellipse->setTransformOriginPoint(m_ellipse->rect().center() - m_ellipse->pos());

    QPen pen = m_ellipse->pen();
    pen.setWidth(0.005 * h);
    m_ellipse->setPen(pen);

    updatePosition();
}


void CalibrationWindow::timerEvent(QTimerEvent * event)
{
    QGraphicsView::timerEvent(event);

    return;

    if(m_running && event->timerId() == m_timer)
    {
        const qint64 elapsed = m_elapsed_timer.restart();
        m_t += m_speed * qreal(elapsed > 100 ? 100 : elapsed);

        updatePosition();
    }
}

void CalibrationWindow::updatePosition()
{
    const QPointF pt =
        QPointF(m_offset_x + m_ampl_x * std::sin(m_coeff_x * m_t + m_coeff_delta),
                m_offset_y + m_ampl_y * std::sin(m_coeff_y * m_t));

    m_ellipse->setPos(pt - QPoint(m_ellipse->rect().width(), m_ellipse->rect().height()));

    if(m_emittingData)
    {
        emit cursorPos(pt, QDateTime::currentMSecsSinceEpoch());
    }
}
*/

CalibrationWindow::CalibrationWindow(QWidget * parent)
    : QWidget(parent)
    , m_running(false)
    , m_emittingData(false)
    , m_t(0.0)
    , m_speed(0.0001)
    , m_margin_x(0.0)
    , m_margin_y(0.0)
    , m_offset_x(1.0)
    , m_offset_y(1.0)
    , m_ampl_x(1.0)
    , m_ampl_y(1.0)
    , m_coeff_x(5.0)
    , m_coeff_y(4.0)
    , m_coeff_delta(0.0)
    , m_circle_size(20.0)
{
    m_pen.setWidth(2);
    m_pen.setColor(Qt::black);
    m_brush.setColor(Qt::yellow);
    m_brush.setStyle(Qt::SolidPattern);

    // 60 fps
    m_timer = startTimer(1000 / 60, Qt::PreciseTimer);

    //setAutoFillBackground(true);
}

void CalibrationWindow::startMovement()
{
    m_running = true;
    m_elapsed_timer.start();
}

void CalibrationWindow::startEmittingData()
{
    m_emittingData = true;
}

void CalibrationWindow::stopMovement()
{
    m_running = false;
}

void CalibrationWindow::stopEmittingData()
{
    m_emittingData = false;
}

void CalibrationWindow::stopAll()
{
    stopMovement();
    stopEmittingData();
}

void CalibrationWindow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);

    const qreal w = width();
    const qreal h = height();
    m_margin_x = 0.05 * w;
    m_margin_y = 0.08 * h;
    m_offset_x = 0.5 * w;
    m_offset_y = 0.5 * h;
    m_ampl_x = 0.5 * (w - 2.0 * m_margin_x);
    m_ampl_y = 0.5 * (h - 2.0 * m_margin_y);
    m_circle_size = 0.025 * h;
    m_circle_size = m_circle_size < 10 ? 10 : m_circle_size;
    m_pen.setWidth(0.004 * h);
}

void CalibrationWindow::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter qp;
    qp.begin(this);
    qp.setRenderHint(QPainter::Antialiasing);

    if(m_running)
    {
        const qint64 elapsed = m_elapsed_timer.restart();
        m_t += m_speed * qreal(elapsed > 100 ? 100 : elapsed);
    }

    const QPointF pt =
        QPointF(m_offset_x + m_ampl_x * std::sin(m_coeff_x * m_t + m_coeff_delta),
                m_offset_y + m_ampl_y * std::sin(m_coeff_y * m_t));

    qp.setPen(m_pen);
    qp.setBrush(m_brush);
    qp.drawEllipse(pt, m_circle_size, m_circle_size);

    qp.end();

    if(m_emittingData)
    {
        emit cursorPos(pt, QDateTime::currentMSecsSinceEpoch());
    }
}

void CalibrationWindow::timerEvent(QTimerEvent * event)
{
    if(m_running && event->timerId() == m_timer)
        repaint();
}
