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


OpenCLAccelerator::OpenCLAccelerator(Objects& objects): m_objects(objects)
{

}


OpenCLAccelerator::~OpenCLAccelerator()
{

}


std::vector<XY> OpenCLAccelerator::forces()
{
    const char kernel_source[] = BOOST_COMPUTE_STRINGIZE_SOURCE
    (

        struct XY
        {
            double x;
            double y;
        };


        double point_distance(double x1, double y1, double x2, double y2)
        {
            const double dist = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );

            return dist;
        }


        struct XY unit_vector(double x1, double y1, double x2, double y2)
        {
            struct XY v;
            v.x = x2 - x1;
            v.y = y2 - y1;

            const double dist = point_distance(x1, y1, x2, y2);

            v.x /= dist;
            v.y /= dist;

            return v;
        }


        __kernel void forces(__global const double* objX,
                             __global const double* objY,
                             __global const double* mass,
                             __global double* forceX,
                             __global double* forceY,
                             const int count
                            )
        {
            const int i = get_global_id(0);
            const double G = 6.6732e-11;

            for(int j = 0; j < count; j++)
            {
                if (i == j)
                    continue;

                const double x1 = objX[i];
                const double y1 = objY[i];
                const double x2 = objX[j];
                const double y2 = objY[j];
                const double m1 = mass[i];
                const double m2 = mass[j];

                const double dist = point_distance(x1, y1, x2, y2);
                const double dist2 = dist * dist;
                const double masses = m1 * m2;
                const double Fg = G * masses / dist2;

                struct XY force_vector = unit_vector(x1, y1, x2, y2);
                force_vector.x *= Fg;
                force_vector.y *= Fg;

                forceX[i] += force_vector.x;
                forceY[i] += force_vector.y;
            }
        }

    );

    const int count = m_objects.size();
    std::vector<double> host_forceX(count);
    std::vector<double> host_forceY(count);

    boost::compute::device device = boost::compute::system::default_device();

    boost::compute::context context(device);
    boost::compute::command_queue queue(context, device);

    boost::compute::buffer objX(context, count * sizeof(double));
    boost::compute::buffer objY(context, count * sizeof(double));
    boost::compute::buffer mass(context, count * sizeof(double));
    boost::compute::buffer forceX(context, count * sizeof(double));
    boost::compute::buffer forceY(context, count * sizeof(double));

    queue.enqueue_write_buffer(objX, 0, count * sizeof(double), m_objects.getX().data());
    queue.enqueue_write_buffer(objY, 0, count * sizeof(double), m_objects.getY().data());
    queue.enqueue_write_buffer(mass, 0, count * sizeof(double), m_objects.getMass().data());
    queue.enqueue_write_buffer(forceX, 0, count * sizeof(double), host_forceX.data());
    queue.enqueue_write_buffer(forceY, 0, count * sizeof(double), host_forceY.data());

    boost::compute::program program = boost::compute::program::create_with_source(kernel_source, context);

    program.build();

    boost::compute::kernel kernel(program, "forces");

    kernel.set_arg(0, objX);
    kernel.set_arg(1, objY);
    kernel.set_arg(2, mass);
    kernel.set_arg(3, forceX);
    kernel.set_arg(4, forceY);
    kernel.set_arg(5, count);

    queue.enqueue_1d_range_kernel(kernel, 0, count, 0);

    queue.enqueue_read_buffer(forceX, 0, count * sizeof(double), host_forceX.data());
    queue.enqueue_read_buffer(forceY, 0, count * sizeof(double), host_forceY.data());

    std::vector<XY> result(count);
    for(int i = 0; i < count; i++)
        result[i] = XY(host_forceX[i], host_forceY[i]);

    return result;
}


std::vector<XY> OpenCLAccelerator::velocities(const std::vector<XY>& forces, double dt) const
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


std::vector<std::pair<int, int>> OpenCLAccelerator::collisions() const
{
    const std::size_t objs = m_objects.size();

    const int threads = 1;
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
                const int tid = 1;
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
