/*
 * Gravity simulation engine
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

#include "simulation_engine.hpp"

namespace
{
    double distance(const Object& o1, const Object& o2)
    {
        const XY& p1 = o1.pos();
        const XY& p2 = o2.pos();
        const double dist = sqrt( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );

        return dist;
    }

    XY unit_vector(const Object& o1, const Object& o2)
    {
        XY v( o1.pos() - o2.pos() );
        const double dist = distance(o1, o2);

        v.x /= dist;
        v.y /= dist;

        return v;
    }
}


SimulationEngine::SimulationEngine()
{

}


SimulationEngine::~SimulationEngine()
{

}


int SimulationEngine::addObject(const Object& obj)
{
    const int id = m_objects.size();
    m_objects.push_back(obj);

    return id;
}


void SimulationEngine::stepBy(double dt)
{
    const double G = 6.6732e-11;

    for(int i = 0; i < m_objects.size() - 1; i++)
        for(int j = i + 1; j < m_objects.size(); j++)
        {
            Object& o1 = m_objects[i];
            Object& o2 = m_objects[j];

            const double dist = distance(o1, o2);
            const double dist2 = dist * dist;
            const double masses = o1.mass() * o2.mass();
            const double Fg = G * masses / dist2;

            XY force_vector = unit_vector(o2, o1);
            force_vector *= Fg;

            o1.addForce(force_vector);
            o2.addForce(-force_vector);
        }

    for(int i = 0; i < m_objects.size(); i++)
        m_objects[i].applyForce(dt);
}


const std::vector< Object >& SimulationEngine::objects() const
{
    return m_objects;
}
