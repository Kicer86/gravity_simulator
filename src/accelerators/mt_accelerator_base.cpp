/*
 * Base for multi thread accelerators
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

#include "mt_accelerator_base.hpp"

#include <omp.h>

#include "../objects.hpp"


MTAcceleratorBase::MTAcceleratorBase(Objects& objects): m_objects(objects)
{

}


MTAcceleratorBase::~MTAcceleratorBase()
{

}


std::vector<XY> MTAcceleratorBase::forces()
{
    const std::size_t objs = m_objects.size();

    std::vector<XY> forces(objs);

    // prepare private tables for threads for results, so we don't get races when accessing 'forces'
    const int threads = omp_get_max_threads();
    std::vector< std::vector<XY> > private_forces(threads);    // for each thread vector of its calculations

    for(int t = 0; t < threads; t++)
        private_forces[t] = std::vector<XY>(objs);             // initialize vector of results for each vector

    #pragma omp parallel for schedule(static, 1)
    for(std::size_t i = 0; i < objs - 1; i++)
    {
        const int tid = omp_get_thread_num();
        forcesFor(i, private_forces[tid]);
    }

    // accumulate results
    for(int t = 0; t < threads; t++)
        for(std::size_t i = 0; i < objs; i++)
            forces[i] += private_forces[t][i];

    return forces;
}


XY MTAcceleratorBase::force(std::size_t i, std::size_t j) const
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


std::vector<XY> MTAcceleratorBase::velocities(const std::vector<XY>& forces, double dt) const
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


std::vector< std::pair< int, int > > MTAcceleratorBase::collisions() const
{
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

    std::vector<std::pair<int, int>> result;

    // collect data from threads into one set of objects to be colided
    for(int t = 0; t < threads; t++)
    {
        const auto& thread_colided = toColide[t];
        for(std::size_t i = 0; i < thread_colided.size(); i++)
            result.push_back( thread_colided[i] );
    }

    return result;
}
