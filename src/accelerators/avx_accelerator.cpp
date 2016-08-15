/*
 * AVX based accelerator for base calculations.
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

#include "avx_accelerator.hpp"

#include "../objects.hpp"


AVXAccelerator::AVXAccelerator(Objects& objects): m_objects(objects), m_dt(60.0)
{

}


AVXAccelerator::~AVXAccelerator()
{

}


double AVXAccelerator::step()
{
    bool optimal = false;

    const std::size_t objs = m_objects.size();

    std::vector<XY> v(objs);
    std::vector<XY> pos(objs);

    const std::vector<XY> forces = calculateForces();

    do
    {
        const std::vector<XY> speeds = calculateVelocities(forces, m_dt);

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

    return m_dt;
}


XY AVXAccelerator::force(std::size_t i, std::size_t j) const
{
    const double G = 6.6732e-11;

    const double x1 = m_objects.getX()[i];
    const double y1 = m_objects.getY()[i];
    const double x2 = m_objects.getX()[j];
    const double y2 = m_objects.getY()[j];
    const double m1 = m_objects.getMass()[i];
    const double m2 = m_objects.getMass()[j];

    const double dist = utils::distance(x1, y1, x2, y2);
    const double dist2 = dist * dist;
    const double masses = m1 * m2;
    const double Fg = G * masses / dist2;

    XY force_vector = utils::unit_vector(x1, y1, x2, y2);
    force_vector *= Fg;

    return force_vector;
}


std::vector<XY> AVXAccelerator::calculateForces() const
{
    const std::size_t objs = m_objects.size();

    std::vector<XY> forces(objs);

    // prepare private tables for threads for results, so we don't get races when accessing 'forces'
    const int threads = 1;
    std::vector< std::vector<XY> > private_forces(threads);    // for each thread vector of its calculations

    for(int t = 0; t < threads; t++)
        private_forces[t] = std::vector<XY>(objs);             // initialize vector of results for each vector

    for(std::size_t i = 0; i < objs - 1; i++)
        for(std::size_t j = i + 1; j < objs; j++)
        {
            const XY force_vector = force(i, j);
            const int tid = 0;

            private_forces[tid][i] += force_vector;
            private_forces[tid][j] += -force_vector;
        }

    // accumulate results
    for(int t = 0; t < threads; t++)
        for(std::size_t i = 0; i < objs; i++)
            forces[i] += private_forces[t][i];

    return forces;
}

std::vector<XY> AVXAccelerator::calculateVelocities(const std::vector<XY>& forces, double dt) const
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
