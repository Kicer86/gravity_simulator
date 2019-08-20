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

#include "cpu_accelerator_base.hpp"

#include <cassert>
#include <omp.h>

#include "../objects.hpp"


CpuAcceleratorBase::CpuAcceleratorBase (Objects* objects): m_objects(objects)
{

}


CpuAcceleratorBase::~CpuAcceleratorBase()
{

}


void CpuAcceleratorBase::setObjects(Objects* objects)
{
    m_objects = objects;
}


std::vector<force_vector_t> CpuAcceleratorBase::forces()
{
    assert(m_objects != nullptr);

    const std::size_t objs = m_objects->size();

    std::vector<force_vector_t> forces(objs);

    // prepare private tables for threads for results, so we don't get races when accessing 'forces'
    const int threads = omp_get_max_threads();
    std::vector< std::vector<force_vector_t> > private_forces(threads);    // for each thread vector of its calculations

    for(int t = 0; t < threads; t++)
        private_forces[t] = std::vector<force_vector_t>(objs);             // initialize vector of results for each vector

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


XY CpuAcceleratorBase::force(std::size_t i, std::size_t j) const
{
    const BaseType G = 6.6732e-11;

    const BaseType x1 = m_objects->getX()[i];
    const BaseType y1 = m_objects->getY()[i];
    const BaseType x2 = m_objects->getX()[j];
    const BaseType y2 = m_objects->getY()[j];
    const BaseType m1 = m_objects->getMass()[i];
    const BaseType m2 = m_objects->getMass()[j];

    const BaseType dist = utils::distance(x1, y1, x2, y2);
    const BaseType dist2 = dist * dist;
    const BaseType Fg = (G * m1) * (m2 / dist2);               // (G * m1) and (m2 / dist2) are here to decrease partial results - for floats "m1 * m2" may be a killer

    XY force_vector = utils::unit_vector(x1, y1, x2, y2);
    force_vector *= Fg;

    return force_vector;
}


std::vector<XY> CpuAcceleratorBase::velocities(const std::vector<force_vector_t>& forces, double dt) const
{
    assert(m_objects != nullptr);

    const std::size_t objs = m_objects->size();
    std::vector<XY> result;
    result.reserve(objs);

    for(std::size_t i = 0; i < objs; i++)
    {
        const XY& dF = forces[i];
        const Object& o = (*m_objects)[i];

        // F=am ⇒ a = F/m
        const XY a = dF / o.mass();

        // ΔV = aΔt
        const XY dv = a * dt;

        result.push_back(dv);
    }

    return result;
}


std::vector< std::pair< int, int > > CpuAcceleratorBase::collisions() const
{
    assert(m_objects != nullptr);

    const std::size_t objs = m_objects->size();

    const int threads = omp_get_max_threads();
    std::vector< std::vector< std::pair<int, int> > > toColide(threads);

    // calculate collisions in parallel
    #pragma omp parallel for schedule(static, 1)
    for(std::size_t i = 0; i < objs - 1; i++)
        for(std::size_t j = i + 1; j < objs; j++)
        {
            const BaseType x1 = m_objects->getX()[i];
            const BaseType y1 = m_objects->getY()[i];
            const BaseType x2 = m_objects->getX()[j];
            const BaseType y2 = m_objects->getY()[j];
            const BaseType r1 = m_objects->getRadius()[i];
            const BaseType r2 = m_objects->getRadius()[j];

            const BaseType dist = utils::distance(x1, y1, x2, y2);

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
