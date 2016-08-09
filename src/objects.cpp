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


#include "objects.hpp"


Objects::Objects()
{

}


Objects::~Objects()
{

}


const std::vector<double>& Objects::getX() const
{
    return m_x;
}


std::vector<double>& Objects::getX()
{
    return m_x;
}

const std::vector<double>& Objects::getY() const
{
    return m_y;
}


std::vector<double>& Objects::getY()
{
    return m_y;
}


const std::vector<double>& Objects::getVX() const
{
    return m_vx;
}


std::vector<double>& Objects::getVX()
{
    return m_vx;
}


const std::vector<double>& Objects::getVY() const
{
    return m_vy;
}


std::vector<double>& Objects::getVY()
{
    return m_vy;
}


const std::vector<double>& Objects::getMass() const
{
    return m_mass;
}


std::vector<double>& Objects::getMass()
{
    return m_mass;
}


const std::vector<double>& Objects::getRadius() const
{
    return m_radius;
}

std::vector<double>& Objects::getRadius()
{
    return m_radius;
}


const std::vector<int>& Objects::getId() const
{
    return m_id;
}


std::vector<int>& Objects::getId()
{
    return m_id;
}
