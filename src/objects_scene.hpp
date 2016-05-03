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

#ifndef OBJECTSSCENE_HPP
#define OBJECTSSCENE_HPP

#include <unordered_map>

#include <QGraphicsScene>

class ObjectsScene: public QGraphicsScene
{
    public:
        ObjectsScene();
        ObjectsScene(const ObjectsScene &) = delete;
        ~ObjectsScene();
        ObjectsScene& operator=(const ObjectsScene &) = delete;

        void addObject(int id);
        void updatePosition(int id, const QPointF &);

    private:
        std::unordered_map<int, QGraphicsItem *> m_objects;
};

#endif // OBJECTSSCENE_HPP
