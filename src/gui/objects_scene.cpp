/*
 * Scene for graviting objects
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

#include "objects_scene.hpp"

#include <cassert>

#include <QColor>
#include <QGraphicsItem>
#include <QPen>


ObjectsScene::ObjectsScene(): m_objects()
{
    const BaseType grid = 400e6;
    QPen pen;
    pen.setWidth(10e6);

    addLine(-grid, 0, grid, 0, pen);
    addLine(0, -grid, 0, grid, pen);
}


ObjectsScene::~ObjectsScene()
{

}


void ObjectsScene::addObject(int id, const Object& obj)
{
    const XY& p = obj.pos();
    const QPointF position(p.x, p.y);

    BaseType radius = obj.radius();

    QGraphicsItem* item = createItem(radius);
    item->setPos(position);

    m_objects.insert( std::make_pair(id, item) );
}


void ObjectsScene::updatePosition(int id, const XY& pos)
{
    const QPointF position(pos.x, pos.y);

    auto obj = m_objects.find(id);
    assert(obj != m_objects.end());

    obj->second->setPos(position);
}


void ObjectsScene::updateRadius(int id, BaseType r)
{
    auto obj = m_objects.find(id);
    assert(obj != m_objects.end());

    QGraphicsItem* item = createItem(r);

    const QPointF pos = obj->second->pos();
    item->setPos(pos);

    // swap items
    removeItem(obj->second);
    delete obj->second;
    obj->second = item;
}


void ObjectsScene::removeObject(int id)
{
    auto obj = m_objects.find(id);
    assert(obj != m_objects.end());

    removeItem(obj->second);

    m_objects.erase(obj);
}


QGraphicsItem* ObjectsScene::createItem(BaseType radius)
{
    const QSizeF size(2 * radius, 2 * radius);
    const QRectF rect(QPointF(-radius, -radius), size);
    QPen pen(Qt::red);
    pen.setWidthF(10e6);

    QGraphicsItem* item = addEllipse(rect, pen, QBrush(Qt::SolidPattern));

    return item;
}
