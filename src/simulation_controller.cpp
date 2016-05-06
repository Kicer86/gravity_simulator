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


SimulationController::SimulationController(): m_engine(), m_timer(), m_scene(nullptr)
{
    connect(&m_timer, &QTimer::timeout, this, &SimulationController::tick);
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
    int id1 = m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );
    int id2 = m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 0, 0) );
    //int id2 = m_engine.addObject( Object(384400e3, 0, 7.347673e22,  1737.1e3, 500, 1.022e3) );
    //int id3 = m_engine.addObject( Object(-384400e3, 0, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );
    //int id4 = m_engine.addObject( Object(-184400e3, 184400e3, 7.347673e22, 1737.1e3, 0.0, -1.022e3) );

    m_scene->addObject(id1, QPointF(0, 0));
    m_scene->addObject(id2, QPointF(0, 0));
    //m_scene->addObject(id3, QPointF(0, 0));
    //m_scene->addObject(id4, QPointF(0, 0));

    m_timer.start(20);
}


void SimulationController::tick()
{
    m_engine.stepBy(3600);

    const std::vector<Object>& objs = m_engine.objects();

    for(int i = 0; i < objs.size(); i++)
        m_scene->updatePosition(i, QPointF(objs[i].pos().x, objs[i].pos().y));
}


void SimulationController::objectsColided(int, int id2)
{
    m_scene->removeObject(id2);
}
