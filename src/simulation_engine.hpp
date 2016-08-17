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
#include <memory>

#include "objects.hpp"

struct IAccelerator;


struct ISimulationEvents
{
    virtual ~ISimulationEvents() {}

    virtual void objectsColided(const Object &, const Object &) = 0;        // first id is for object which became bigger, second id is for object that was annihilated
    virtual void objectCreated(int id, const Object &) = 0;
    virtual void objectAnnihilated(const Object &) = 0;
    virtual void objectUpdated(int id, const Object &) = 0;
};


class SimulationEngine
{
    public:
        SimulationEngine();
        SimulationEngine(const SimulationEngine &) = delete;
        ~SimulationEngine();

        SimulationEngine& operator=(const SimulationEngine &) = delete;

        void addEventsObserver(ISimulationEvents *);

        int addObject(const Object &);
        int stepBy(double);
        double step();

        const Objects& objects() const;
        std::size_t objectCount() const;

    private:
        Objects m_objects;
        std::vector<ISimulationEvents *> m_eventObservers;
        std::unique_ptr<IAccelerator> m_accelerator;
        double m_dt;
        int m_nextId;

        std::size_t collide(std::size_t, std::size_t);
        void checkForCollisions();
};

#endif // SIMULATIONENGINE_HPP
