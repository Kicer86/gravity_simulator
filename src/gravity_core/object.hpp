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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "types.hpp"


struct XY
{
    BaseType x;
    BaseType y;

    XY(BaseType _x, BaseType _y);
    XY();

    XY operator-(const XY& other) const;
    XY& operator*=(BaseType v);
    XY operator*(BaseType v) const;
    XY& operator+=(const XY& other);
    XY operator+(const XY& other) const;
    XY operator-() const;
    XY operator/(BaseType v) const;
};


template<typename Unit>
struct vector2d
{
    Unit x, y;

    vector2d(Unit _x, Unit _y): x(_x), y(_y) { }
    vector2d(): x(0.0), y(0.0){ }
    vector2d(const XY& xy): x(xy.x), y(xy.y) {}

    vector2d operator-(const vector2d& other) const
    {
        vector2d result = *this;
        result.x -= other.x;
        result.y -= other.y;

        return result;
    }

    vector2d& operator*=(BaseType v)
    {
        x *= v;
        y *= v;

        return *this;
    }

    vector2d operator*(BaseType v) const
    {
        vector2d result(x, y);
        result *= v;

        return result;
    }

    vector2d& operator+=(const vector2d& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    vector2d operator+(const vector2d& other) const
    {
        vector2d result(x, y);
        result += other;

        return result;
    }

    vector2d operator-() const
    {
        vector2d result(-x, -y);

        return result;
    }

    vector2d operator/(BaseType v) const
    {
        XY result = *this;
        result.x /= v;
        result.y /= v;

        return result;
    }
};


class Object
{
        XY m_pos;
        XY m_v;
        BaseType m_mass;
        BaseType m_radius;

        int m_id;                  // object id used for object identification

        Object(BaseType x, BaseType y, BaseType m, BaseType r, BaseType v_x, BaseType v_y, int id);

        friend class Objects;

    public:
        Object(BaseType x, BaseType y, BaseType m, BaseType r, BaseType v_x = 0.0, BaseType v_y = 0.0);

        BaseType mass() const;
        BaseType radius() const;
        const XY& pos() const;
        const XY& velocity() const;
        int id() const;
};

namespace utils
{
    BaseType distance(const XY &, const XY &);
    BaseType distance(BaseType x1, BaseType y1,
                               BaseType x2, BaseType y2);

    XY unit_vector(const XY &, const XY &);
    XY unit_vector(BaseType x1, BaseType y1,
                   BaseType x2, BaseType y2);
}


#endif // OBJECT_HPP
