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

#ifndef OPENCLACCELERATOR_HPP
#define OPENCLACCELERATOR_HPP

#include <boost/compute/core.hpp>

#include "iaccelerator.hpp"

class Objects;

class OpenCLAccelerator: public IAccelerator
{
    public:
        OpenCLAccelerator(Objects * = nullptr);
        OpenCLAccelerator(const OpenCLAccelerator &) = delete;
        ~OpenCLAccelerator();

        OpenCLAccelerator& operator=(const OpenCLAccelerator &) = delete;

        virtual void setObjects(Objects *) override;

        virtual std::vector<force_vector_t> forces() override;
        virtual std::vector<XY> velocities(const std::vector<force_vector_t>& forces, time_type dt) const override;
        virtual std::vector<std::pair<int, int>> collisions() const override;

    private:
        Objects* m_objects;
        boost::compute::program m_program;
        boost::compute::context m_context;
        boost::compute::device  m_device;
};

#endif // OPENCLACCELERATOR_HPP
