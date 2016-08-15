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

#include "accelerators/openmp_accelerator.hpp"


SimulationEngine::SimulationEngine():
    m_objects(),
    m_eventObservers(),
    m_accelerator(nullptr),
    m_nextId(1)                        // 0 is reserved for invalid entry
{
    m_accelerator = std::make_unique<OpenMPAccelerator>(m_objects);
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
    const double dt = m_accelerator->step();

    checkForCollisions();

    return dt;
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

    const std::size_t objs = m_objects.size();

    const int threads = omp_get_max_threads();
    std::vector< std::vector< std::pair<int, int> > > toColide(threads);

    // calculate collisions in parallel
    #pragma omp parallel for schedule(static, 1)
    for(std::size_t i = 0; i < objs - 1; i++)
        for(std::size_t j = i + 1; j < objs; j++)
        {
            const double x1 = m_objects.getX()[i];
            const double y1 = m_objects.getY()[i];
            const double x2 = m_objects.getX()[j];
            const double y2 = m_objects.getY()[j];
            const double r1 = m_objects.getRadius()[i];
            const double r2 = m_objects.getRadius()[j];

            const double dist = utils::distance(x1, y1, x2, y2);

            if ( (r1 + r2) > dist)
            {
                const int tid = omp_get_thread_num();
                const auto colided = std::make_pair(i, j);
                toColide[tid].push_back(colided);
            }

        }

    // collect data from threads into one set of objects to be removed
    for(int t = 0; t < threads; t++)
    {
        const auto& thread_colided = toColide[t];
        for(std::size_t i = 0; i < thread_colided.size(); i++)
        {
            const auto& colided = thread_colided[i];

            const int idx1 = colided.first;
            const int idx2 = colided.second;

            const Object& ob1 = m_objects[idx1];
            const Object& ob2 = m_objects[idx2];

            const int id1 = ob1.id();
            const int id2 = ob2.id();

            if (toRemove.find(id1) == toRemove.end() && toRemove.find(id2) == toRemove.end())
            {
                const int destroyed = collide(idx1, idx2);
                toRemove.insert(destroyed);
            }
        }
    }

    // remove destroyed objects (remember to go from farthest objects)
    for(std::size_t i: toRemove)
        m_objects.erase(i);
}
