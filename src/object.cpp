/*
 * Object - container for physical data
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

#include "object.hpp"

#include <cmath>


XY::XY(double _x, double _y): x(_x), y(_y)
{

}


XY::XY(): x(0.0), y(0.0)
{

}


XY XY::operator-(const XY& other) const
{
    XY result = *this;
    result.x -= other.x;
    result.y -= other.y;

    return result;
}


XY& XY::operator*=(double v)
{
    x *= v;
    y *= v;

    return *this;
}


XY XY::operator*(double v) const
{
    XY result(x, y);
    result *= v;

    return result;
}


XY& XY::operator+=(const XY& other)
{
    x += other.x;
    y += other.y;

    return *this;
}


XY XY::operator+(const XY& other) const
{
    XY result(x, y);
    result += other;

    return result;
}


XY XY::operator-() const
{
    XY result(-x, -y);

    return result;
}


XY XY::operator/(double v) const
{
    XY result = *this;
    result.x /= v;
    result.y /= v;

    return result;
}


///////////////////////////////////////////////////////////////////////////////

Object::Object(double x, double y, double m, double r, double v_x, double v_y):
    m_pos(x, y),
    m_v(v_x, v_y),
    m_mass(m),
    m_radius(r),
    m_id(0)
{

}


double Object::mass() const
{
    return m_mass;
}


double Object::radius() const
{
    return m_radius;
}


const XY& Object::pos() const
{
    return m_pos;
}


const XY& Object::velocity() const
{
    return m_v;
}


int Object::id() const
{
    return m_id;
}

void Object::setId(int id)
{
    m_id = id;
}


void Object::setMass(double m)
{
    m_mass = m;
}


void Object::setRadius(double r)
{
    m_radius = r;
}


void Object::setVelocity(const XY& v)
{
    m_v = v;
}


void Object::setPos(const XY& p)
{
    m_pos = p;
}


///////////////////////////////////////////////////////////////////////////////


namespace utils
{
    double distance(const XY& p1, const XY& p2)
    {
        const double dist = sqrt( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );

        return dist;
    }

    double distance(const Object& o1, const Object& o2)
    {
        const XY& p1 = o1.pos();
        const XY& p2 = o2.pos();
        const double dist = distance(p1, p2);

        return dist;
    }

    XY unit_vector(const XY& p1, const XY& p2)
    {
        XY v( p1 - p2 );
        const double dist = distance(p1, p2);

        v.x /= dist;
        v.y /= dist;

        return v;
    }

    XY unit_vector(const Object& o1, const Object& o2)
    {
        const XY v = unit_vector(o1.pos(), o2.pos());

        return v;
    }
}
