/*
 * Simulation presets
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

#include "simulation_preset.hpp"

#include <cmath>
#include <cstdint>
#include <limits>


namespace
{
    constexpr BaseType GRAVITATIONAL_CONSTANT = 6.67430e-11;
    constexpr BaseType PI = 3.14159265358979323846;


    BaseType randomBetween(std::uint32_t& seed, BaseType min, BaseType max)
    {
        seed = seed * 1664525u + 1013904223u;

        const BaseType normalized = static_cast<BaseType>(seed) / static_cast<BaseType>(std::numeric_limits<std::uint32_t>::max());

        return min + normalized * (max - min);
    }


    BaseType circularVelocity(BaseType centralMass, BaseType radius)
    {
        return std::sqrt(GRAVITATIONAL_CONSTANT * centralMass / radius);
    }


    std::vector<Object> moonCloud()
    {
        std::vector<Object> objects;
        objects.reserve(2000);

        std::uint32_t seed = 3;

        for (int i = 0; i < 2000; i++)
        {
            const BaseType x = randomBetween(seed, -5000e6, 5000e6);
            const BaseType y = randomBetween(seed, -5000e6, 5000e6);

            const BaseType v_x = randomBetween(seed, -5e2, 5e2);
            const BaseType v_y = randomBetween(seed, -5e2, 5e2);

            objects.emplace_back(x, y, 7.347673e24, 1737.1e3, v_x, v_y);
        }

        return objects;
    }


    std::vector<Object> fourMoonCross()
    {
        return
        {
            Object(0, 0, 5.9736e24, 6371e3),
            Object(384400e3, 0, 7.347673e22, 1737.1e3, 500, 1.022e3),
            Object(-384400e3, 0, 7.347673e22, 1737.1e3, 0.0, -1.022e3),
            Object(-184400e3, 184400e3, 7.347673e22, 1737.1e3, 0.0, -1.022e3),
        };
    }


    std::vector<Object> moonChain()
    {
        std::vector<Object> objects;
        objects.reserve(32);
        objects.emplace_back(0, 0, 5.9736e24, 6371e3);

        for (int i = 1; i < 32; i++)
            objects.emplace_back(384400e3 * i / 10, 0, 7.347673e22, 1737.1e3, 0, 1.022e3 * (i % 2? 1: -1));

        return objects;
    }


    std::vector<Object> earthAndMoon()
    {
        return
        {
            Object(0, 0, 5.9736e24, 6371e3),
            Object(384400e3, 0, 7.347673e22, 1737.1e3, 0, 1.022e3),
        };
    }


    std::vector<Object> solarSystem()
    {
        return
        {
            Object(0, 0, 1.98847e30, 696340e3),
            Object(57.9e9, 0, 3.3011e23, 2439.7e3, 0, 47.36e3),
            Object(108.2e9, 0, 4.8675e24, 6051.8e3, 0, 35.02e3),
            Object(149.6e9, 0, 5.97237e24, 6371e3, 0, 29.78e3),
            Object(227.9e9, 0, 6.4171e23, 3389.5e3, 0, 24.07e3),
            Object(778.6e9, 0, 1.8982e27, 69911e3, 0, 13.07e3),
            Object(1433.5e9, 0, 5.6834e26, 58232e3, 0, 9.68e3),
            Object(2872.5e9, 0, 8.6810e25, 25362e3, 0, 6.80e3),
            Object(4495.1e9, 0, 1.02413e26, 24622e3, 0, 5.43e3),
        };
    }


    std::vector<Object> binaryStars()
    {
        return
        {
            Object(-40e9, 0, 1.2e30, 540000e3, 0, -28.0e3),
            Object(40e9, 0, 1.2e30, 540000e3, 0, 28.0e3),
            Object(0, 220e9, 5.97237e24, 6371e3, -28.0e3, 0),
            Object(0, -320e9, 6.4171e23, 3389.5e3, 22.0e3, 0),
            Object(430e9, 0, 1.8982e27, 69911e3, 0, 20.0e3),
        };
    }


    std::vector<Object> asteroidRing()
    {
        std::vector<Object> objects;
        objects.reserve(97);

        const BaseType centralMass = 1.98847e30;
        objects.emplace_back(0, 0, centralMass, 696340e3);

        for (int i = 0; i < 96; i++)
        {
            const BaseType angle = 2.0 * PI * i / 96.0;
            const BaseType radius = 120e9 + (i % 12) * 9e9;
            const BaseType speed = circularVelocity(centralMass, radius);

            const BaseType x = std::cos(angle) * radius;
            const BaseType y = std::sin(angle) * radius;
            const BaseType v_x = -std::sin(angle) * speed;
            const BaseType v_y = std::cos(angle) * speed;

            objects.emplace_back(x, y, 3.0e20, 900e3, v_x, v_y);
        }

        return objects;
    }
}


const std::vector<SimulationPreset>& simulationPresets()
{
    static const std::vector<SimulationPreset> presets =
    {
        {
            QStringLiteral("Moon chain"),
            QStringLiteral("Earth with a line of alternating moons copied from the previous default #if branch."),
            moonChain(),
        },
        {
            QStringLiteral("Earth and Moon"),
            QStringLiteral("A compact two-body baseline with one moon in orbit around Earth."),
            earthAndMoon(),
        },
        {
            QStringLiteral("Four moon cross"),
            QStringLiteral("Earth with three moon-sized bodies approaching from different directions."),
            fourMoonCross(),
        },
        {
            QStringLiteral("Moon cloud"),
            QStringLiteral("Two thousand moon-mass bodies spread through a large square with random velocities."),
            moonCloud(),
        },
        {
            QStringLiteral("Solar system"),
            QStringLiteral("The Sun and eight planets with approximate average orbital distances and speeds."),
            solarSystem(),
        },
        {
            QStringLiteral("Binary stars"),
            QStringLiteral("Two similar stars orbiting their common center with a few larger bodies nearby."),
            binaryStars(),
        },
        {
            QStringLiteral("Asteroid ring"),
            QStringLiteral("A star surrounded by a dense ring of small bodies on circular trajectories."),
            asteroidRing(),
        },
    };

    return presets;
}


const SimulationPreset& defaultSimulationPreset()
{
    return simulationPresets().front();
}
