/*
 * Gravity simulation engine
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


#ifndef SIMULATIONENGINE_HPP
#define SIMULATIONENGINE_HPP


#include <cmath>
#include <iostream>
#include <vector>


struct XY
{
    double x;
    double y;

    XY(double _x, double _y): x(_x), y(_y) {}
    XY(): x(0.0), y(0.0) {}

    XY operator-(const XY& other) const
    {
        XY result = *this;
        result.x -= other.x;
        result.y -= other.y;

        return result;
    }

    XY& operator*=(double v)
    {
        x *= v;
        y *= v;

        return *this;
    }

    XY operator*(double v) const
    {
        XY result(x, y);
        result *= v;

        return result;
    }

    XY& operator+=(const XY& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    XY operator-() const
    {
        XY result(-x, -y);

        return result;
    }

    XY& operator/(double v)
    {
        x /= v;
        y /= v;

        return *this;
    }
};


class Object
{
        XY m_pos;
        XY m_v;
        double m_mass;

        XY dF;

    public:
        Object(double x, double y, double m, double v_x = 0.0, double v_y = 0.0): m_pos(x, y), m_v(v_x, v_y), m_mass(m) {}

        double mass() const
        {
            return m_mass;
        }

        const XY& pos() const
        {
            return m_pos;
        }

        const XY& velocity() const
        {
            return m_v;
        }

        void addForce(const XY& f)
        {
            dF += f;
        }

        void applyForce(double dt)
        {
            // F=am ⇒ a = F/m
            const XY a = dF/m_mass;

            // ΔV = aΔt
            const XY dv = a * dt;

            m_v += dv;
            m_pos += m_v * dt;

            dF.x = 0.0;
            dF.y = 0.0;
        }
};


class SimulationEngine
{
    public:
        SimulationEngine();
        SimulationEngine(const SimulationEngine &) = delete;
        ~SimulationEngine();

        SimulationEngine& operator=(const SimulationEngine &) = delete;

        void addObject(const Object &);
        void stepBy(double);

        const std::vector<Object>& objects() const;

    private:
        std::vector<Object> m_objects;
};

#endif // SIMULATIONENGINE_HPP
