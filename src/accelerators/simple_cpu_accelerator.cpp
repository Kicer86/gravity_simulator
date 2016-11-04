/*
 * Cpu based accelerator for calculations.
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

#include "simple_cpu_accelerator.hpp"

#include "../objects.hpp"


SimpleCpuAccelerator::SimpleCpuAccelerator (Objects& objects): MTAcceleratorBase(objects)
{

}


SimpleCpuAccelerator::~SimpleCpuAccelerator()
{

}


void SimpleCpuAccelerator::forcesFor(std::size_t i, std::vector<XY>& forces) const
{
    const std::size_t count = m_objects.size();

    for(std::size_t j = i + 1; j < count; j++)
    {
        const XY force_vector = force(i, j);

        forces[i] += force_vector;
        forces[j] += -force_vector;
    }
}
