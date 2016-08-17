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

#include <cassert>

#include "objects_scene.hpp"

bool equal(double l, double r)
{
    return std::abs(l - r) < 0.00001;
}

double fRand(double fMin, double fMax)
{
    double f = static_cast<double>(rand()) / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


Tick::Tick():
    colided(),
    created(),
    annihilated(),
    updated(),
    colidedMutex(),
    createdMutex(),
    annihilatedMutex(),
    updatedMutex()
{

}



Tick::Tick(const Tick& other):
    colided(),
    created(),
    annihilated(),
    updated(),
    colidedMutex(),
    createdMutex(),
    annihilatedMutex(),
    updatedMutex()
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
    m_stepTimer(),
    m_calculationsThread(),
    m_tickData(),
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

    m_stepTimer.setInterval(1000/50);
    m_stepTimer.moveToThread(&m_calculationsThread);

    connect(&m_stepTimer, &QTimer::timeout, this, &SimulationController::tick, Qt::DirectConnection);  // make sure tick will be called from calculations thread
    connect(&m_calculationsThread, SIGNAL(started()), &m_stepTimer, SLOT(start()));
    connect(&m_calculationsThread, SIGNAL(finished()), &m_stepTimer, SLOT(stop()));
    connect(this, &SimulationController::tickData, this, &SimulationController::updateScene);          // inter-thread communication signal
}


SimulationController::~SimulationController()
{
    m_calculationsThread.quit();
    m_calculationsThread.wait();
}


void SimulationController::setScene(ObjectsScene* scene)
{
    m_scene = scene;
}


void SimulationController::beginSimulation()
{
#if 1
    srand(3);

    for (int i = 0; i < 2000; i++)
    {
        const double x = fRand(-5000e6, 5000e6);
        const double y = fRand(-5000e6, 5000e6);

        const double v_x = fRand(-5e2, 5e2);
        const double v_y = fRand(-5e2, 5e2);

        m_engine.addObject( Object(x, y, 7.347673e24, 1737.1e3, v_x, v_y) );
        //int id2 = m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 500, 1.022e3) );
        //int id3 = m_engine.addObject( Object(-384400e3, 0, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
        //int id4 = m_engine.addObject( Object(-184400e3, 184400e3, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
    }
#elif 0
    m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );
    m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 500, 1.022e3) );
    m_engine.addObject( Object(-384400e3, 0, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
    m_engine.addObject( Object(-184400e3, 184400e3, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
#elif 1
    m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );

    for(int i = 1; i < 16; i++)
        m_engine.addObject( Object(384400e3 * i/10, 0, 7.347673e22,  1737.1e3, 0, 1.022e3) );
#elif 0
    m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );
    m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 0, 1.022e3) );
#endif

#if 0
    SimulationEngine engine1(false);
    SimulationEngine engine2(true);

    engine1.addObject( Object(0, 0, 5.9736e24, 6371e3) );

    for(int i = 1; i < 8; i++)
        engine1.addObject( Object(384400e3 * i/10, 0, 7.347673e22,  1737.1e3, 0, 1.022e3) );

    engine2.addObject( Object(0, 0, 5.9736e24, 6371e3) );

    for(int i = 1; i < 8; i++)
        engine2.addObject( Object(384400e3 * i/10, 0, 7.347673e22,  1737.1e3, 0, 1.022e3) );

    for(int i = 0; ;i++)
    {
        engine1.step();
        engine2.step();

        const Objects& obj1 = engine1.objects();
        const Objects& obj2 = engine2.objects();

        assert( obj1.size() == obj2.size() );
        for(std::size_t j = 0; j < obj1.size(); j++)
        {
            assert( obj1.getId()[j] == obj2.getId()[j] );
            assert( equal(obj1.getX()[j], obj2.getX()[j]) );
            assert( equal(obj1.getY()[j], obj2.getY()[j]) );
            assert( equal(obj1.getVX()[j], obj2.getVX()[j]) );
            assert( equal(obj1.getVY()[j], obj2.getVY()[j]) );
            assert( equal(obj1.getMass()[j], obj2.getMass()[j]) );
            assert( equal(obj1.getRadius()[j], obj2.getRadius()[j]) );
        }
    }
#endif

    // before starting simulation update scene
    const auto& objects = m_engine.objects();
    for (std::size_t i = 0; i < objects.size(); i++)
    {
        const Object obj = objects[i];
        m_scene->addObject(obj.id(), obj);
    }

    m_calculationsThread.start();
}


int SimulationController::fps() const
{
    return m_fps;
}


void SimulationController::tick()
{
    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    m_tickData.clear();
    const int steps = m_engine.stepBy(1800);
    m_framesCounter++;

    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    const auto diff = end - start;
    const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    std::cout << "Tick time: " << diff_ms << ", steps: " << steps << std::endl;

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
    std::lock_guard<std::mutex> lockCreated(m_tickData.createdMutex);
    m_tickData.created.push_back( obj );
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
