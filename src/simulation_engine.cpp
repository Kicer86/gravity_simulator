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

#include <cassert>
#include <set>

#include <omp.h>

#include "accelerators/avx_accelerator.hpp"
#include "accelerators/cpu_accelerator.hpp"


SimulationEngine::SimulationEngine():
    m_objects(),
    m_eventObservers(),
    m_accelerator(nullptr),
    m_dt(60.0),
    m_nextId(1)                        // 0 is reserved for invalid entry
{
    m_accelerator = std::make_unique<AVXAccelerator>(m_objects);
}


SimulationEngine::~SimulationEngine()
{

}


void SimulationEngine::addEventsObserver(ISimulationEvents* observer)
{
    m_eventObservers.push_back(observer);
}


int SimulationEngine::addObject(const Object& obj)
{
    assert(obj.id() == 0);
    const auto idx =  m_objects.insert(obj, m_nextId);
    const Object addedObj = m_objects[idx];

    for(ISimulationEvents* events: m_eventObservers)
        events->objectCreated(m_nextId, addedObj);

    return m_nextId++;
}


int SimulationEngine::stepBy(double dt)
{
    int steps = 0;

    while (dt > 0.0)
    {
        dt -= step();
        steps++;
    }

    for (std::size_t i = 0; i < m_objects.size(); i++)
        for(ISimulationEvents* events: m_eventObservers)
        {
            const Object obj = m_objects[i];
            events->objectUpdated(obj.id(), obj);
        }

    return steps;
}


double SimulationEngine::step()
{
    bool optimal = false;

    const std::size_t objs = m_objects.size();

    std::vector<XY> v(objs);
    std::vector<XY> pos(objs);

    const std::vector<XY> forces = m_accelerator->forces();

    do
    {
        const std::vector<XY> speeds = m_accelerator->velocities(forces, m_dt);

        // figure out maximum distance made by single object
        double max_travel = 0.0;

        for(std::size_t i = 0; i < objs; i++)
        {
            Object o = m_objects[i];

            const XY& dV = speeds[i];
            v[i] = dV + o.velocity();
            pos[i] = o.pos() + v[i] * m_dt;

            const double travel = utils::distance(pos[i], o.pos());

            if (travel > max_travel)
                max_travel = travel;
        }

        // do not allow too big jumps (precission loss) nor no small ones (performance loss)
        if (max_travel > 100e3)
            m_dt = m_dt * 100e3 / max_travel;
        else if (max_travel < 1e3)
            m_dt = m_dt * 1e3 / max_travel;
        else
            optimal = true;
    }
    while(optimal == false);

    // apply new positions and speeds
    for(std::size_t i = 0; i < objs; i++)
    {
        m_objects.setPos(i, pos[i]);
        m_objects.setVelocity(i, v[i]);
    }

    checkForCollisions();

    return m_dt;
}


const Objects& SimulationEngine::objects() const
{
    return m_objects;
}


std::size_t SimulationEngine::objectCount() const
{
    return m_objects.size();
}


std::size_t SimulationEngine::collide(std::size_t i, std::size_t j)
{
    assert(i < m_objects.size());
    assert(j < m_objects.size());

    const double mass1 = m_objects.getMass()[i];
    const double mass2 = m_objects.getMass()[j];

    const std::size_t heavier = mass1 > mass2? i: j;
    const std::size_t lighter = heavier == i? j : i;

    Object h = m_objects[heavier];
    Object l = m_objects[lighter];

    // correct velocity by summing momentums
    const double masses = h.mass() + l.mass();
    const XY momentums = h.velocity() * h.mass() + l.velocity() * l.mass();
    const XY newVelocity = momentums / masses;

    m_objects.setVelocity(heavier, newVelocity);

    // increase mass and radius
    const double newRadius = std::cbrt( std::pow( h.radius(), 3 ) + std::pow( l.radius(), 3 ) );

    m_objects.setMass(heavier, masses);
    m_objects.setRadius(heavier, newRadius);

    for(ISimulationEvents* events: m_eventObservers)
        events->objectsColided(h, l);

    for(ISimulationEvents* events: m_eventObservers)
        events->objectAnnihilated(l);

    return lighter;
}


void SimulationEngine::checkForCollisions()
{
    // Container for object to be removed.
    // It is neccesary to keep objects in right order (from greater idx to lower one).
    // Without it erase algorithm may lead to serious errors
    // (object is erased by being overwriten with last one).
    std::set<std::size_t, std::greater<std::size_t>> toRemove;

    std::vector<std::pair<int, int>> toColide = m_accelerator->collisions();

    for(std::size_t i = 0; i < toColide.size(); i++)
    {
        const auto& colided = toColide[i];

        const int idx1 = colided.first;
        const int idx2 = colided.second;

        const Object ob1 = m_objects[idx1];
        const Object ob2 = m_objects[idx2];

        const int id1 = ob1.id();
        const int id2 = ob2.id();

        if (toRemove.find(id1) == toRemove.end() && toRemove.find(id2) == toRemove.end())
        {
            const int destroyed = collide(idx1, idx2);
            toRemove.insert(destroyed);
        }
    }

    // remove destroyed objects (remember to go from farthest objects)
    for(std::size_t i: toRemove)
        m_objects.erase(i);
}
