/*
 * Simulation Controller
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

#ifndef SIMULATIONCONTROLLER_HPP
#define SIMULATIONCONTROLLER_HPP

#include <atomic>
#include <deque>
#include <mutex>

#include <QTimer>
#include <QThread>

#include "simulation_engine.hpp"

#include "accelerators/avx_accelerator.hpp"

class ObjectsScene;

struct Tick
{
    std::deque< std::pair<Object, Object> > colided;
    std::deque<Object> created;
    std::deque<Object> annihilated;
    std::deque<Object> updated;

    mutable std::mutex colidedMutex;
    mutable std::mutex createdMutex;
    mutable std::mutex annihilatedMutex;
    mutable std::mutex updatedMutex;

    Tick();
    Tick(const Tick &);
    Tick& operator=(const Tick &);

    void clear();
};

Q_DECLARE_METATYPE(Tick)


class SimulationController: public QObject, ISimulationEvents
{
	Q_OBJECT

    public:
        SimulationController();
        SimulationController(const SimulationController &) = delete;
        ~SimulationController();
        SimulationController& operator=(const SimulationController &) = delete;

        void setScene(ObjectsScene *);

        void beginSimulation();
        int fps() const;

    private:
        AVXAccelerator m_accelerator;
        SimulationEngine m_engine;
        QTimer m_stepTimer;
        QThread m_calculationsThread;
        Tick m_tickData;
        ObjectsScene* m_scene;
        int m_fps;
        std::atomic<int> m_framesCounter;

        void tick();
        void updateScene(const Tick &);

        // ISimulationEvents:
        virtual void objectsColided(const Object&, const Object&) override;
        virtual void objectCreated(int id, const Object &) override;
        virtual void objectAnnihilated(const Object &) override;
        virtual void objectUpdated(int id, const Object &) override;

    signals:
        void fpsUpdated(int);
        void objectCountUpdated(int);
        void tickData(const Tick &);
};

#endif // SIMULATIONCONTROLLER_HPP
