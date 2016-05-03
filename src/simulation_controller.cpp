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


SimulationController::SimulationController(): m_engine(), m_scene(nullptr)
{

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
    int id1 = m_engine.addObject( Object(0, 0, 5.9736e24) );
    int id2 = m_engine.addObject( Object(6373e3, 0, 1e0, 0.0, 19000) );

    m_scene->addObject(id1, QPointF(0, 0));
    m_scene->addObject(id2, QPointF(6373e3, 0));
}
