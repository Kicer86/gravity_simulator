/*
 * Objects View
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "objects_view.hpp"

#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>

#include "simulation_controller.hpp"


ObjectsView::ObjectsView(QWidget* p): QGraphicsView(p), m_prevPoint()
{

}


ObjectsView::~ObjectsView()
{

}


void ObjectsView::mousePressEvent(QMouseEvent* event)
{
    m_prevPoint = event->pos();
}


void ObjectsView::mouseMoveEvent(QMouseEvent* event)
{
    if ( (event->buttons() & Qt::LeftButton) > 0 )
    {
        const QPoint curPoint = event->pos();
        const QPoint diff = curPoint - m_prevPoint;

        auto scrollBy = [](QScrollBar* scrollBar, int dx)
        {
            const int v = scrollBar->value();
            const int n_v = v + dx;

            if (n_v <= scrollBar->maximum() && n_v >= scrollBar->minimum())
                scrollBar->setValue(v + dx);
        };

        scrollBy(horizontalScrollBar(), -diff.x());
        scrollBy(verticalScrollBar(), -diff.y());

        m_prevPoint = curPoint;
    }
}


void ObjectsView::mouseReleaseEvent(QMouseEvent* event)
{
    m_prevPoint = QPoint();
}


void ObjectsView::wheelEvent(QWheelEvent* event)
{
    const QPoint delta = event->angleDelta();

    if (delta.y() > 0)
        scale(1.2, 1.2);
    else if (delta.y() < 0)
        scale(0.82, 0.82);
}


void ObjectsView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
}


void ObjectsView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);

    QGraphicsScene* s = scene();
    if (s != nullptr)
    {
        const QRectF r = s->sceneRect();
        fitInView(r, Qt::KeepAspectRatio);
    }
}
