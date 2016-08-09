/*
 * Container for Objects
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

#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <vector>

#include "object.hpp"

class Objects
{
    public:
        Objects();
        Objects(const Objects &) = delete;
        ~Objects();

        Objects& operator=(const Objects &) = delete;

        const std::vector<double>& getX() const;
        std::vector<double>& getX();

        const std::vector<double>& getY() const;
        std::vector<double>& getY();

        const std::vector<double>& getVX() const;
        std::vector<double>& getVX();

        const std::vector<double>& getVY() const;
        std::vector<double>& getVY();

        const std::vector<double>& getMass() const;
        std::vector<double>& getMass();

        const std::vector<double>& getRadius() const;
        std::vector<double>& getRadius();

        const std::vector<int>& getId() const;
        std::vector<int>& getId();

    private:
        // objects data
        std::vector<double> m_x;
        std::vector<double> m_y;
        std::vector<double> m_vx;
        std::vector<double> m_vy;
        std::vector<double> m_mass;
        std::vector<double> m_radius;
        std::vector<int> m_id;
};

#endif // OBJECTS_HPP
