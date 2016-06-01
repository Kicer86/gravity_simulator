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

#ifndef OBJECTSVIEW_HPP
#define OBJECTSVIEW_HPP

#include <QGraphicsView>

class ObjectsView: public QGraphicsView
{
    public:
        ObjectsView(QWidget  *);
        ObjectsView(const ObjectsView &) = delete;
        ~ObjectsView();        
        ObjectsView& operator=(const ObjectsView &) = delete;
        
    protected:
        QPoint m_prevPoint;
        
        virtual void mousePressEvent(QMouseEvent *);
        virtual void mouseMoveEvent(QMouseEvent *);
        virtual void mouseReleaseEvent(QMouseEvent *);
        virtual void wheelEvent(QWheelEvent *);
        virtual void resizeEvent(QResizeEvent *);
        virtual void showEvent(QShowEvent *);
};

#endif // OBJECTSVIEW_HPP
