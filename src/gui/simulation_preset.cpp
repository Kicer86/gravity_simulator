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

#include <cstdint>
#include <limits>


namespace
{
    BaseType randomBetween(std::uint32_t& seed, BaseType min, BaseType max)
    {
        seed = seed * 1664525u + 1013904223u;

        const BaseType normalized = static_cast<BaseType>(seed) / static_cast<BaseType>(std::numeric_limits<std::uint32_t>::max());

        return min + normalized * (max - min);
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
    };

    return presets;
}


const SimulationPreset& defaultSimulationPreset()
{
    return simulationPresets().front();
}
