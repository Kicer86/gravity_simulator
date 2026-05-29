/*
 * Simulation preset dialog
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

#include "simulation_preset_dialog.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include <QColor>
#include <QCommandLinkButton>
#include <QFont>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "simulation_preset.hpp"


namespace
{
    QColor colorForObject(const Object& object)
    {
        const BaseType mass = object.mass().raw_value();

        if (mass > 1.0e29)
            return QColor(255, 202, 76);
        else if (mass > 1.0e26)
            return QColor(94, 174, 255);
        else if (mass > 1.0e23)
            return QColor(157, 211, 132);
        else
            return QColor(217, 217, 224);
    }


    int radiusForObject(const Object& object)
    {
        const BaseType mass = object.mass().raw_value();

        if (mass > 1.0e29)
            return 7;
        else if (mass > 1.0e26)
            return 5;
        else if (mass > 1.0e23)
            return 4;
        else
            return 2;
    }


    QIcon createPresetIcon(const SimulationPreset& preset)
    {
        const QSize size(112, 72);
        QPixmap pixmap(size);
        pixmap.fill(QColor(9, 11, 20));

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QPointF center(size.width() / 2.0, size.height() / 2.0);
        BaseType maxExtent = 1.0;

        for (const Object& object: preset.objects)
        {
            const XY& pos = object.pos();
            maxExtent = std::max(maxExtent, static_cast<BaseType>(std::abs(pos.x)));
            maxExtent = std::max(maxExtent, static_cast<BaseType>(std::abs(pos.y)));
        }

        const BaseType scale = static_cast<BaseType>(0.42 * std::min(size.width(), size.height())) / maxExtent;

        painter.setPen(QPen(QColor(70, 76, 98), 1));
        painter.drawEllipse(center, scale * maxExtent, scale * maxExtent);

        std::vector<const Object*> objects;
        objects.reserve(preset.objects.size());

        for (const Object& object: preset.objects)
            objects.push_back(&object);

        std::sort(objects.begin(), objects.end(), [](const Object* left, const Object* right)
        {
            return left->mass().raw_value() < right->mass().raw_value();
        });

        for (const Object* object: objects)
        {
            const XY& pos = object->pos();
            const QPointF point(center.x() + pos.x * scale, center.y() - pos.y * scale);
            const int radius = radiusForObject(*object);

            painter.setPen(Qt::NoPen);
            painter.setBrush(colorForObject(*object));
            painter.drawEllipse(point, radius, radius);
        }

        return QIcon(pixmap);
    }
}


SimulationPresetDialog::SimulationPresetDialog(QWidget* parent):
    QDialog(parent),
    m_selectedPreset(0)
{
    setWindowTitle(QStringLiteral("Choose initial setup"));
    resize(760, 560);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel(QStringLiteral("Choose initial setup"), this);
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    title->setFont(titleFont);
    mainLayout->addWidget(title);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QWidget* container = new QWidget(scrollArea);
    QGridLayout* presetsLayout = new QGridLayout(container);
    presetsLayout->setColumnStretch(0, 1);
    presetsLayout->setColumnStretch(1, 1);

    const std::vector<SimulationPreset>& presets = simulationPresets();
    for (std::size_t i = 0; i < presets.size(); i++)
    {
        const SimulationPreset& preset = presets[i];

        QCommandLinkButton* presetButton = new QCommandLinkButton(preset.name, preset.description, container);
        presetButton->setIcon(createPresetIcon(preset));
        presetButton->setIconSize(QSize(112, 72));
        presetButton->setMinimumHeight(118);
        presetButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        connect(presetButton, &QCommandLinkButton::clicked, this, [this, i]
        {
            m_selectedPreset = static_cast<int>(i);
            accept();
        });

        presetsLayout->addWidget(presetButton, static_cast<int>(i / 2), static_cast<int>(i % 2));
    }

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
}


const SimulationPreset& SimulationPresetDialog::selectedPreset() const
{
    return simulationPresets().at(m_selectedPreset);
}
