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


Tick::Tick()
{

}



Tick::Tick(const Tick& other)
{
    *this = other;
}


Tick& Tick::operator=(const Tick& other)
{
    std::lock_guard<std::mutex> lockColided(other.colidedMutex);
    std::lock_guard<std::mutex> lockCreated(other.createdMutex);
    std::lock_guard<std::mutex> lockAnnihilated(other.annihilatedMutex);
    std::lock_guard<std::mutex> lockUpdated(other.updatedMutex);

    colided = other.colided;
    created = other.created;
    annihilated = other.annihilated;
    updated = other.updated;

    return *this;
}


void Tick::clear()
{
    std::lock_guard<std::mutex> lockColided(colidedMutex);
    std::lock_guard<std::mutex> lockCreated(createdMutex);
    std::lock_guard<std::mutex> lockAnnihilated(annihilatedMutex);
    std::lock_guard<std::mutex> lockUpdated(updatedMutex);

    colided.clear();
    created.clear();
    annihilated.clear();
    updated.clear();
}


SimulationController::SimulationController():
    m_engine(),
    m_timer(),
    m_calculations(),
    m_scene(nullptr),
    m_fps(0),
    m_framesCounter(0)
{
    qRegisterMetaType<Tick>();

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

    m_timer.setInterval(1000/50);
    m_timer.moveToThread(&m_calculations);

    connect(&m_timer, &QTimer::timeout, this, &SimulationController::tick, Qt::DirectConnection);
    connect(&m_calculations, SIGNAL(started()), &m_timer, SLOT(start()));
    connect(&m_calculations, SIGNAL(finished()), &m_timer, SLOT(stop()));
    connect(this, &SimulationController::tickData, this, &SimulationController::updateScene);
}


SimulationController::~SimulationController()
{
    m_calculations.quit();
    m_calculations.wait();
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

        m_engine.addObject( Object(x, y, 7.347673e24, 1737.1e3, v_x, v_y) );
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

    m_calculations.start();
}


int SimulationController::fps() const
{
    return m_fps;
}


void SimulationController::tick()
{
    m_tickData.clear();
    m_engine.stepBy(180);
    m_framesCounter++;

    emit tickData(m_tickData);
}


void SimulationController::updateScene(const Tick& data)
{
    for(const auto& objs: data.colided)
    {
        const Object& obj = objs.first;
        m_scene->updateRadius(obj.id(), obj.radius());
    }

    for(const Object& obj: data.created)
        m_scene->addObject(obj.id(), obj);

    for(const Object& obj: data.annihilated)
        m_scene->removeObject(obj.id());

    for(const Object& obj: data.updated)
        m_scene->updatePosition(obj.id(), obj.pos());

    emit objectCountUpdated(data.updated.size());
}


void SimulationController::objectsColided(const Object& obj1, const Object& obj2)
{
    std::lock_guard<std::mutex> lockColided(m_tickData.colidedMutex);
    m_tickData.colided.push_back( std::make_pair(obj1, obj2) );
}


void SimulationController::objectCreated(int, const Object& obj)
{
    // Not quite nice... When called from main thread (m_calculations is not running)
    // pass event directly to scene, otherewise collect it
    if (m_calculations.isRunning())
    {
        std::lock_guard<std::mutex> lockCreated(m_tickData.createdMutex);
        m_tickData.created.push_back( obj );
    }
    else
        m_scene->addObject(obj.id(), obj);
}


void SimulationController::objectAnnihilated(const Object& obj)
{
    std::lock_guard<std::mutex> lockAnnihilated(m_tickData.annihilatedMutex);
    m_tickData.annihilated.push_back( obj );
}


void SimulationController::objectUpdated(int, const Object& obj)
{
    std::lock_guard<std::mutex> lockUpdated(m_tickData.updatedMutex);
    m_tickData.updated.push_back( obj );
}
