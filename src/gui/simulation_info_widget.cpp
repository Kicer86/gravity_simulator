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
#include "types.hpp"

#include <QGridLayout>
#include <QLabel>


SimulationInfoWidget::SimulationInfoWidget(QWidget* p):
    QWidget(p),
    m_fpsValue(nullptr),
    m_objCountValue(nullptr)
{
    QGridLayout* mainLayout = new QGridLayout(this);

    QLabel* fpsLabel = new QLabel(tr("fps:"), this);
    QLabel* objCountLabel = new QLabel(tr("objects:"), this);
    QLabel* idLabel = new QLabel(tr("object id:"), this);
    QLabel* posLabel = new QLabel(tr("position:"), this);
    QLabel* radiusLabel = new QLabel(tr("radius:"), this);
    QLabel* massLabel = new QLabel(tr("mass:"), this);

    m_fpsValue = new QLabel(this);
    m_objCountValue = new QLabel(this);
    m_objIDValue = new QLabel(this);
    m_objPosValue = new QLabel(this);
    m_objRadiusValue = new QLabel(this);
    m_objMassValue = new QLabel(this);

    mainLayout->addWidget(fpsLabel, 0, 0);
    mainLayout->addWidget(m_fpsValue, 0, 1);
    mainLayout->addWidget(objCountLabel, 1, 0);
    mainLayout->addWidget(m_objCountValue, 1, 1);
    mainLayout->addWidget(idLabel, 2, 0);
    mainLayout->addWidget(m_objIDValue, 2, 1);
    mainLayout->addWidget(posLabel, 3, 0);
    mainLayout->addWidget(m_objPosValue, 3, 1);
    mainLayout->addWidget(radiusLabel, 4, 0);
    mainLayout->addWidget(m_objRadiusValue, 4, 1);
    mainLayout->addWidget(massLabel, 5, 0);
    mainLayout->addWidget(m_objMassValue, 5, 1);
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

void SimulationInfoWidget::updateObjectData(const QGraphicsItem *item)
{
    if (item == nullptr)
    {
        m_objIDValue->setText(QString(""));
        m_objPosValue->setText(QString(""));
        m_objRadiusValue->setText(QString(""));
        m_objMassValue->setText(QString(""));
    }
    else
    {
        const int id = item->data(ObjectData::Id).toInt();
        const QPointF pos = item->pos();
        const BaseType mass = item->data(ObjectData::Mass).value<BaseType>();
        const BaseType radius = item->data(ObjectData::Radius).value<BaseType>();

        m_objIDValue->setText(QString::number(id));
        m_objPosValue->setText(QString("%1, %2").arg(pos.x(), 0, 'g', 2)
                           .arg(pos.y(), 0, 'g', 2));
        m_objRadiusValue->setText(QString::number(radius, 'g', 3));
        m_objMassValue->setText(QString::number(mass, 'g', 3));
    }
}

