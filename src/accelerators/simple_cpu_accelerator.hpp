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

#ifndef SIMPLECPUACCELERATOR_HPP
#define SIMPLECPUACCELERATOR_HPP

#include <vector>

#include "mt_accelerator_base.hpp"
#include "../object.hpp"

class Objects;

class SimpleCpuAccelerator: public CpuAcceleratorBase
{
    public:
        SimpleCpuAccelerator (Objects &);
        SimpleCpuAccelerator (const SimpleCpuAccelerator &) = delete;
        ~SimpleCpuAccelerator();
        SimpleCpuAccelerator& operator=(const SimpleCpuAccelerator &) = delete;

    private:
        virtual void forcesFor(std::size_t, std::vector<XY> &) const override;
};

#endif // SIMPLECPUACCELERATOR_HPP
