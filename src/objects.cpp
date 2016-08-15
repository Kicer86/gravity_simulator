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

#include <cassert>


#define INITIAL_SIZE 10000


Objects::Objects():
    m_x(),
    m_y(),
    m_vx(),
    m_vy(),
    m_mass(),
    m_radius(),
    m_id()
{
    m_x.reserve(INITIAL_SIZE);
    m_y.reserve(INITIAL_SIZE);
    m_vx.reserve(INITIAL_SIZE);
    m_vy.reserve(INITIAL_SIZE);
    m_mass.reserve(INITIAL_SIZE);
    m_radius.reserve(INITIAL_SIZE);
    m_id.reserve(INITIAL_SIZE);
}


Objects::~Objects()
{

}


Object Objects::operator[](std::size_t idx) const
{
    const Object obj(m_x[idx], m_y[idx], m_mass[idx], m_radius[idx], m_vx[idx], m_vy[idx], m_id[idx]);

    return obj;
}


std::size_t Objects::size() const
{
    return m_id.size();
}


std::size_t Objects::insert(const Object& obj, std::size_t id)
{
    const std::size_t idx = m_id.size();

    m_x.push_back(obj.pos().x);
    m_y.push_back(obj.pos().y);
    m_vx.push_back(obj.velocity().x);
    m_vy.push_back(obj.velocity().y);
    m_mass.push_back(obj.mass());
    m_radius.push_back(obj.radius());
    m_id.push_back(id);

    assert(m_x.size() == m_id.size());
    assert(m_y.size() == m_id.size());
    assert(m_vx.size() == m_id.size());
    assert(m_vy.size() == m_id.size());
    assert(m_mass.size() == m_id.size());
    assert(m_radius.size() == m_id.size());

    return idx;
}


void Objects::erase(std::size_t idx)
{
    const std::size_t last = size() - 1;

    m_x[idx]      = m_x[last];
    m_y[idx]      = m_y[last];
    m_vx[idx]     = m_vx[last];
    m_vy[idx]     = m_vy[last];
    m_mass[idx]   = m_mass[last];
    m_radius[idx] = m_radius[last];
    m_id[idx]     = m_id[last];

    m_x.pop_back();
    m_y.pop_back();
    m_vx.pop_back();
    m_vy.pop_back();
    m_mass.pop_back();
    m_radius.pop_back();
    m_id.pop_back();

    assert(m_x.size() == m_id.size());
    assert(m_y.size() == m_id.size());
    assert(m_vx.size() == m_id.size());
    assert(m_vy.size() == m_id.size());
    assert(m_mass.size() == m_id.size());
    assert(m_radius.size() == m_id.size());
}


void Objects::setPos(std::size_t idx, const XY& xy)
{
    m_x[idx] = xy.x;
    m_y[idx] = xy.y;
}


void Objects::setVelocity(std::size_t idx, const XY& vxy)
{
    m_vx[idx] = vxy.x;
    m_vy[idx] = vxy.y;
}


void Objects::setMass(std::size_t idx, double mass)
{
    m_mass[idx] = mass;
}


void Objects::setRadius(std::size_t idx, double radius)
{
    m_radius[idx] = radius;
}


XY Objects::getPos(std::size_t idx) const
{
    const XY xy(m_x[idx], m_y[idx]);

    return xy;
}


XY Objects::getVelocity(std::size_t idx) const
{
    const XY vxy(m_vx[idx], m_vy[idx]);

    return vxy;
}


const Objects::DataVector& Objects::getX() const
{
    return m_x;
}


Objects::DataVector& Objects::getX()
{
    return m_x;
}

const Objects::DataVector& Objects::getY() const
{
    return m_y;
}


Objects::DataVector& Objects::getY()
{
    return m_y;
}


const Objects::DataVector& Objects::getVX() const
{
    return m_vx;
}


Objects::DataVector& Objects::getVX()
{
    return m_vx;
}


const Objects::DataVector& Objects::getVY() const
{
    return m_vy;
}


Objects::DataVector& Objects::getVY()
{
    return m_vy;
}


const Objects::DataVector& Objects::getMass() const
{
    return m_mass;
}


Objects::DataVector& Objects::getMass()
{
    return m_mass;
}


const Objects::DataVector& Objects::getRadius() const
{
    return m_radius;
}

Objects::DataVector& Objects::getRadius()
{
    return m_radius;
}


const Objects::DataVector& Objects::getId() const
{
    return m_id;
}


Objects::DataVector& Objects::getId()
{
    return m_id;
}
