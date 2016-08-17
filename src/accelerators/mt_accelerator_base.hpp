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

#ifndef MTACCELERATOR_BASE_HPP
#define MTACCELERATOR_BASE_HPP

#include <vector>

#include "iaccelerator.hpp"
#include "../object.hpp"

class Objects;

class MTAcceleratorBase: public IAccelerator
{
    public:
        MTAcceleratorBase(Objects &);
        MTAcceleratorBase(const MTAcceleratorBase &) = delete;
        ~MTAcceleratorBase();
        MTAcceleratorBase& operator=(const MTAcceleratorBase &) = delete;

        virtual double step() final;

    protected:
        Objects& m_objects;

        XY force(std::size_t, std::size_t) const;
        virtual std::vector< std::pair<int, int> > collisions() const final;

        std::vector<XY> calculateForces() const;
        std::vector<XY> calculateVelocities(const std::vector<XY>& forces, double dt) const;

        virtual void forcesFor(std::size_t, std::size_t, std::vector<XY> &) const = 0;

    private:
        double m_dt;
};

#endif // MTACCELERATOR_BASE_HPP
