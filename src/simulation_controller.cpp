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


#include "simulation_controller.hpp"

#include "objects_scene.hpp"

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


SimulationController::SimulationController(): m_engine(), m_timer(), m_scene(nullptr), m_fps(0), m_framesCounter(0)
{
    connect(&m_timer, &QTimer::timeout, this, &SimulationController::tick);

    m_engine.addEventsObserver(this);

    QTimer* fpsTimer = new QTimer(this);
    fpsTimer->setInterval(1000);

    connect(fpsTimer, &QTimer::timeout, [this]
    {
        m_fps = m_framesCounter;
        m_framesCounter = 0;

        emit fpsUpdated(m_fps);
    });

    fpsTimer->start();
}


SimulationController::~SimulationController()
{

}


void SimulationController::setScene(ObjectsScene* scene)
{
    m_scene = scene;
}


void SimulationController::beginSimulation()
{
#if 1
    srand(3);

    for (int i = 0; i < 400; i++)
    {
        const double x = fRand(-500e6, 500e6);
        const double y = fRand(-500e6, 500e6);

        const double v_x = fRand(-5e2, 5e2);
        const double v_y = fRand(-5e2, 5e2);

        int id = m_engine.addObject( Object(x, y, 7.347673e24, 1737.1e3, v_x, v_y) );
        //int id2 = m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 500, 1.022e3) );
        //int id3 = m_engine.addObject( Object(-384400e3, 0, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
        //int id4 = m_engine.addObject( Object(-184400e3, 184400e3, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
    }
#elif 0
    int id1 = m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );
    int id2 = m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 500, 1.022e3) );
    int id3 = m_engine.addObject( Object(-384400e3, 0, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
    int id4 = m_engine.addObject( Object(-184400e3, 184400e3, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
#elif 0
    int id1 = m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );
    int id2 = m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 0, 1.022e3) );
#endif

    m_timer.start(20);
}


int SimulationController::fps() const
{
    return m_fps;
}


void SimulationController::tick()
{
    m_engine.stepBy(180);

    m_framesCounter++;
}


void SimulationController::objectsColided(const Object& obj1, const Object &)
{
    m_scene->updateRadius(obj1.id(), obj1.radius());
}


void SimulationController::objectCreated(int id, const Object& obj)
{
    m_scene->addObject(id, obj);
}


void SimulationController::objectAnnihilated(const Object& obj)
{
    m_scene->removeObject(obj.id());
}


void SimulationController::objectUpdated(int id, const Object& obj)
{
    m_scene->updatePosition(id, obj.pos());
}

