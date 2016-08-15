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


struct XY
{
    double x;
    double y;

    XY(double _x, double _y);
    XY();

    XY operator-(const XY& other) const;
    XY& operator*=(double v);
    XY operator*(double v) const;
    XY& operator+=(const XY& other);
    XY operator+(const XY& other) const;
    XY operator-() const;
    XY operator/(double v) const;

};


class Object
{
        XY m_pos;
        XY m_v;
        double m_mass;
        double m_radius;

        int m_id;                  // object id used for object identification

        Object(double x, double y, double m, double r, double v_x, double v_y, int id);

        friend class Objects;

    public:
        Object(double x, double y, double m, double r, double v_x = 0.0, double v_y = 0.0);

        double mass() const;
        double radius() const;
        const XY& pos() const;
        const XY& velocity() const;
        int id() const;
};

namespace utils
{
    double distance(const XY &, const XY &);
    double distance(const Object &, const Object &);
    XY unit_vector(const XY &, const XY &);
    XY unit_vector(const Object &, const Object &);
}


#endif // OBJECT_HPP
