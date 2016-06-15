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

#include <set>

#include <omp.h>

namespace
{
    double distance(const XY& p1, const XY& p2)
    {
        const double dist = sqrt( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );

        return dist;
    }

    double distance(const Object& o1, const Object& o2)
    {
        const XY& p1 = o1.pos();
        const XY& p2 = o2.pos();
        const double dist = distance(p1, p2);

        return dist;
    }

    XY unit_vector(const XY& p1, const XY& p2)
    {
        XY v( p1 - p2 );
        const double dist = distance(p1, p2);

        v.x /= dist;
        v.y /= dist;

        return v;
    }

    XY unit_vector(const Object& o1, const Object& o2)
    {
        const XY v = unit_vector(o1.pos(), o2.pos());

        return v;
    }
}


SimulationEngine::SimulationEngine():
    m_objects(),
    m_eventObservers(),
    m_dt(60),
    m_nextId(0)
{

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
    m_objects.push_back(obj);
    Object& addedObj = m_objects.back();

    addedObj.setId(m_nextId);

    for(ISimulationEvents* events: m_eventObservers)
        events->objectCreated(m_nextId, addedObj);

    return m_nextId++;
}


void SimulationEngine::stepBy(double dt)
{
    while (dt > 0.0)
        dt -= step();

    for (const Object& obj: m_objects)
        for(ISimulationEvents* events: m_eventObservers)
            events->objectUpdated(obj.id(), obj);
}


double SimulationEngine::step()
{
    bool optimal = false;

    const std::size_t objs = m_objects.size();

    std::vector<XY> v(objs);
    std::vector<XY> pos(objs);

    const std::vector<XY> forces = calculateForces();

    do
    {
        const std::vector<XY> speeds = calculateVelocities(forces, m_dt);

        double max_travel = 0.0;

        for(std::size_t i = 0; i < objs; i++)
        {
            const Object& o = m_objects[i];

            const XY& dV = speeds[i];
            v[i] = dV + o.velocity();
            pos[i] = o.pos() + v[i] * m_dt;

            const double travel = distance(pos[i], o.pos());

            if (travel > max_travel)
                max_travel = travel;
        }

        if (max_travel > 100e3)
            m_dt = m_dt * 100e3 / max_travel;
        else if (max_travel < 1e3)
            m_dt = m_dt * 1e3 / max_travel;
        else
            optimal = true;
    }
    while(optimal == false);

    for(std::size_t i = 0; i < objs; i++)
    {
        Object& o = m_objects[i];
        o.setPos(pos[i]);
        o.setVelocity(v[i]);
    }

    checkForCollisions();

    return m_dt;
}


const std::vector<Object>& SimulationEngine::objects() const
{
    return m_objects;
}


std::size_t SimulationEngine::objectCount() const
{
    return m_objects.size();
}


std::size_t SimulationEngine::collide(std::size_t i, std::size_t j)
{
    Object& o1 = m_objects[i];
    Object& o2 = m_objects[j];

    const std::size_t heavier = o1.mass() > o2.mass()? i: j;
    const std::size_t lighter = heavier == i? j : i;

    Object& h = m_objects[heavier];
    Object& l = m_objects[lighter];

    // correct velocity by summing momentums
    const double masses = h.mass() + l.mass();
    const XY momentums = h.velocity() * h.mass() + l.velocity() * l.mass();
    const XY newVelocity = momentums / masses;

    h.setVelocity(newVelocity);

    // increase mass and radius
    const double newRadius = std::cbrt( std::pow( h.radius(), 3 ) + std::pow( l.radius(), 3 ) );

    h.setMass( masses );
    h.setRadius( newRadius );

    for(ISimulationEvents* events: m_eventObservers)
        events->objectsColided(h, l);

    for(ISimulationEvents* events: m_eventObservers)
        events->objectAnnihilated(l);

    return lighter;
}


void SimulationEngine::checkForCollisions()
{
    std::set<std::size_t, std::greater<std::size_t>> toRemove;

    const std::size_t objs = m_objects.size();

    for(std::size_t i = 0; i < objs - 1; i++)
        for(std::size_t j = i + 1; j < objs; j++)
        {
            const Object& o1 = m_objects[i];
            const Object& o2 = m_objects[j];

            const double dist = distance(o1, o2);

            if (
                (o1.radius() + o2.radius()) > dist &&
                toRemove.find(i) == toRemove.end() &&
                toRemove.find(j) == toRemove.end()
               )
            {
                const int destroyed = collide(i, j);
                toRemove.insert(destroyed);
            }
        }

    for(std::size_t i: toRemove)
    {
        // remove object 'i' by overriding it with last one
        if (i < m_objects.size() - 1)
            m_objects[i] = m_objects.back();

        m_objects.pop_back();
    }
}


std::vector<XY> SimulationEngine::calculateForces() const
{
    const std::size_t objs = m_objects.size();
    const double G = 6.6732e-11;

    std::vector<XY> forces(objs);

    // prepare private tables for threads for results, so we don't get races when accessing 'forces'
    const int threads = omp_get_max_threads();
    std::vector< std::vector<XY> > private_forces(threads);    // for each thread vector of its calculations

    for(int t = 0; t < threads; t++)
        private_forces[t] = std::vector<XY>(objs);             // initialize vector of results for each vector

    #pragma omp parallel for schedule(static, 1)
    for(std::size_t i = 0; i < objs - 1; i++)
        for(std::size_t j = i + 1; j < objs; j++)
        {
            const Object& o1 = m_objects[i];
            const Object& o2 = m_objects[j];

            const double dist = distance(o1, o2);
            const double dist2 = dist * dist;
            const double masses = o1.mass() * o2.mass();
            const double Fg = G * masses / dist2;

            XY force_vector = unit_vector(o2, o1);
            force_vector *= Fg;

            const int tid = omp_get_thread_num();

            private_forces[tid][i] += force_vector;
            private_forces[tid][j] += -force_vector;
        }

    // accumulate results
    for(int t = 0; t < threads; t++)
        for(std::size_t i = 0; i < objs; i++)
            forces[i] += private_forces[t][i];

    return forces;
}


std::vector<XY> SimulationEngine::calculateVelocities(const std::vector<XY>& forces, double dt) const
{
    std::vector<XY> result;
    result.reserve(m_objects.size());

    for(std::size_t i = 0; i < m_objects.size(); i++)
    {
        const XY& dF = forces[i];
        const Object& o = m_objects[i];

        // F=am ⇒ a = F/m
        const XY a = dF / o.mass();

        // ΔV = aΔt
        const XY dv = a * dt;

        result.push_back(dv);
    }

    return result;
}
