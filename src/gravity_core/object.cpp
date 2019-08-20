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


XY::XY(BaseType _x, BaseType _y): x(_x), y(_y)
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


XY& XY::operator*=(BaseType v)
{
    x *= v;
    y *= v;

    return *this;
}


XY XY::operator*(BaseType v) const
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


XY XY::operator/(BaseType v) const
{
    XY result = *this;
    result.x /= v;
    result.y /= v;

    return result;
}


///////////////////////////////////////////////////////////////////////////////

Object::Object(BaseType x, BaseType y, BaseType m, BaseType r, BaseType v_x, BaseType v_y, int id):
    m_pos(x, y),
    m_v(v_x, v_y),
    m_mass(m),
    m_radius(r),
    m_id(id)
{

}


Object::Object(BaseType x, BaseType y, BaseType m, BaseType r, BaseType v_x, BaseType v_y):
    Object(x, y, m, r, v_x, v_y, 0)
{

}


BaseType Object::mass() const
{
    return m_mass;
}


BaseType Object::radius() const
{
    return m_radius;
}


const XY& Object::pos() const
{
    return m_pos;
}


const velocity_vector_t& Object::velocity() const
{
    return m_v;
}


int Object::id() const
{
    return m_id;
}


///////////////////////////////////////////////////////////////////////////////


namespace utils
{
    BaseType distance(const XY& p1, const XY& p2)
    {
        return distance(p1.x, p1.y, p2.x, p2.y);
    }


    BaseType distance(BaseType x1, BaseType y1, BaseType x2, BaseType y2)
    {
        const BaseType dist = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );

        return dist;
    }


    XY unit_vector(const XY& p1, const XY& p2)
    {
        return unit_vector(p1.x, p1.y, p2.x, p2.y);
    }


    XY unit_vector(BaseType x1, BaseType y1, BaseType x2, BaseType y2)
    {
        XY v( x2 - x1, y2 - y1);
        const BaseType dist = distance(x1, y1, x2, y2);

        v.x /= dist;
        v.y /= dist;

        return v;
    }
}
