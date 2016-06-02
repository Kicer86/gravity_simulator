/*
 * Widget with ifnformation about simulation
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


#include "simulation_info_widget.hpp"

#include <QGridLayout>
#include <QLabel>


SimulationInfoWidget::SimulationInfoWidget(QWidget* p): QWidget(p)
{
    QGridLayout* mainLayout = new QGridLayout(this);

    QLabel* fpsLabel = new QLabel(tr("fps:"), this);
    QLabel* objCountLabel = new QLabel(tr("objects:"), this);
    m_fpsValue = new QLabel(this);
    m_objCountValue = new QLabel(this);

    mainLayout->addWidget(fpsLabel, 0, 0);
    mainLayout->addWidget(m_fpsValue, 0, 1);
    mainLayout->addWidget(objCountLabel, 1, 0);
    mainLayout->addWidget(m_objCountValue, 1, 1);
}


SimulationInfoWidget::~SimulationInfoWidget()
{

}


void SimulationInfoWidget::updateFps(int fps)
{
    m_fpsValue->setText(QString::number(fps));
}


void SimulationInfoWidget::updateObjectCount(int count)
{
    m_objCountValue->setText(QString::number(count));
}

