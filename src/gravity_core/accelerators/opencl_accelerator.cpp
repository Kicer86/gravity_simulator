/*
 * OpenCL based accelerator for base calculations.
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

#include "opencl_accelerator.hpp"

#include "omp.h"

#define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION

#include <boost/compute/core.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/algorithm/accumulate.hpp>
#include <boost/compute/algorithm/for_each.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/functional/math.hpp>
#include <boost/compute/functional/operator.hpp>

#include "../objects.hpp"

// tutorials:
// https://anteru.net/blog/2012/11/03/2009/


OpenCLAccelerator::OpenCLAccelerator(Objects* objects):
    m_objects(objects),
    m_program(),
    m_context(),
    m_device()
{
    const char kernel_source[] = BOOST_COMPUTE_STRINGIZE_SOURCE
    (
        struct XY
        {
            float x;
            float y;
        };


        float point_distance(float x1, float y1, float x2, float y2)
        {
            const float dist = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );

            return dist;
        }


        struct XY unit_vector(float x1, float y1, float x2, float y2)
        {
            struct XY v;
            v.x = x2 - x1;
            v.y = y2 - y1;

            const float dist = point_distance(x1, y1, x2, y2);

            v.x /= dist;
            v.y /= dist;

            return v;
        }


        __kernel void forces(__global const float* objX,
                             __global const float* objY,
                             __global const float* mass,
                             __global float* forceX,
                             __global float* forceY,
                             const int count
                            )
        {
            const int i = get_global_id(0);
            const float G = 6.6732e-11;

            if (i < count)
            {
                forceX[i] = 0;
                forceY[i] = 0;

                for(int j = 0; j < count; j++)
                {
                    if (i == j)
                        continue;

                    const float x1 = objX[i];
                    const float y1 = objY[i];
                    const float x2 = objX[j];
                    const float y2 = objY[j];
                    const float m1 = mass[i];
                    const float m2 = mass[j];

                    const float dist = point_distance(x1, y1, x2, y2);
                    const float dist2 = dist * dist;
                    const float Fg = (G * m1) * (m2 / dist2);

                    struct XY force_vector = unit_vector(x1, y1, x2, y2);
                    force_vector.x *= Fg;
                    force_vector.y *= Fg;

                    forceX[i] += force_vector.x;
                    forceY[i] += force_vector.y;
                }
            }
        }
    );

    m_device = boost::compute::system::default_device();
    m_context = boost::compute::context(m_device);
    m_program = boost::compute::program::create_with_source(kernel_source, m_context);

    m_program.build();
}


OpenCLAccelerator::~OpenCLAccelerator()
{

}


void OpenCLAccelerator::setObjects(Objects* objects)
{
    m_objects = objects;
}


std::vector<XY> OpenCLAccelerator::forces()
{
    const int count = m_objects->size();
    std::vector<float> host_forceX(count);
    std::vector<float> host_forceY(count);

    boost::compute::command_queue queue(m_context, m_device);

    boost::compute::buffer objX(m_context, count * sizeof(float), boost::compute::buffer::read_only);
    boost::compute::buffer objY(m_context, count * sizeof(float), boost::compute::buffer::read_only);
    boost::compute::buffer mass(m_context, count * sizeof(float), boost::compute::buffer::read_only);
    boost::compute::buffer forceX(m_context, count * sizeof(float), boost::compute::buffer::write_only);
    boost::compute::buffer forceY(m_context, count * sizeof(float), boost::compute::buffer::write_only);

    auto objXFuture = queue.enqueue_write_buffer_async(objX, 0, count * sizeof(float), m_objects->getX().data());
    auto objYFuture = queue.enqueue_write_buffer_async(objY, 0, count * sizeof(float), m_objects->getY().data());
    auto massFuture = queue.enqueue_write_buffer_async(mass, 0, count * sizeof(float), m_objects->getMass().data());

    boost::compute::kernel kernel(m_program, "forces");

    kernel.set_arg(0, objX);
    kernel.set_arg(1, objY);
    kernel.set_arg(2, mass);
    kernel.set_arg(3, forceX);
    kernel.set_arg(4, forceY);
    kernel.set_arg(5, count);

    objXFuture.wait();
    objYFuture.wait();
    massFuture.wait();

    const std::size_t global_size = count % 64 == 0? count: (count + 64) & (-64);

    queue.enqueue_nd_range_kernel(kernel,
                                  boost::compute::extents<1>(0),
                                  boost::compute::extents<1>(global_size),
                                  boost::compute::extents<1>(64));

    auto forceXReadFuture = queue.enqueue_read_buffer_async(forceX, 0, count * sizeof(float), host_forceX.data());
    auto forceYReadFuture = queue.enqueue_read_buffer_async(forceY, 0, count * sizeof(float), host_forceY.data());

    forceXReadFuture.wait();
    forceYReadFuture.wait();

    std::vector<XY> result(count);
    for(int i = 0; i < count; i++)
        result[i] = XY(host_forceX[i], host_forceY[i]);

    return result;
}


std::vector<XY> OpenCLAccelerator::velocities(const std::vector<XY>& forces, double dt) const
{
    std::vector<XY> result;
    result.reserve(m_objects->size());

    for(std::size_t i = 0; i < m_objects->size(); i++)
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


std::vector<std::pair<int, int>> OpenCLAccelerator::collisions() const
{
    const std::size_t objs = m_objects->size();

    const int threads = omp_get_max_threads();
    std::vector< std::vector< std::pair<int, int> > > toColide(threads);

    // calculate collisions in parallel
    #pragma omp parallel for schedule(static, 1)
    for(std::size_t i = 0; i < objs - 1; i++)
        for(std::size_t j = i + 1; j < objs; j++)
        {
            const float x1 = m_objects->getX()[i];
            const float y1 = m_objects->getY()[i];
            const float x2 = m_objects->getX()[j];
            const float y2 = m_objects->getY()[j];
            const float r1 = m_objects->getRadius()[i];
            const float r2 = m_objects->getRadius()[j];

            const float dist = utils::distance(x1, y1, x2, y2);

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
