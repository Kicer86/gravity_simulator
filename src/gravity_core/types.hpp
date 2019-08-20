
#ifndef TYPES_HPP
#define TYPES_HPP

#include "../libs/si/include/SI/force.h"

typedef float BaseType;

// units
typedef SI::newton_t<BaseType> newton_t;

// quantities
typedef SI::velocity_t<BaseType, std::ratio<1>> velocity_t;

namespace ObjectData
{
    enum
    {
        Id,
        Mass,
        Radius,
        Speed,
    };
}

#endif
