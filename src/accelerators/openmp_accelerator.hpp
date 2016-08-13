/*
 * OpenMP based accelerator for base calculations.
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

#ifndef OPENMPACCELERATOR_H
#define OPENMPACCELERATOR_H

#include "iaccelerator.hpp"

#include <vector>

#include "../simulation_engine.hpp"

class OpenMPAccelerator: public IAccelerator
{
    public:
        OpenMPAccelerator(std::vector<Object> &);
        OpenMPAccelerator(const OpenMPAccelerator &) = delete;
        ~OpenMPAccelerator();
        OpenMPAccelerator& operator=(const OpenMPAccelerator &);

        virtual double step();

    private:
        std::vector<Object>& m_objects;
        double m_dt;

        std::vector<XY> calculateForces() const;
        std::vector<XY> calculateVelocities(const std::vector<XY> &, double) const;
};

#endif // OPENMPACCELERATOR_HPP
