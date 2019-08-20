
#ifndef TYPES_HPP
#define TYPES_HPP

#include "../libs/si/include/SI/force.h"
#include "../libs/si/include/SI/momentum.h"

typedef float BaseType;

// units
typedef SI::newton_t<BaseType> newton_t;

// quantities
typedef SI::velocity_t<BaseType, std::ratio<1>> velocity_type;
typedef SI::acceleration_t<BaseType, std::ratio<1>> acceleration_type;
typedef SI::time_t<BaseType, std::ratio<1>> time_type;
typedef SI::mass_t<BaseType, std::ratio<1>> mass_type;
typedef SI::momentum_t<BaseType, std::ratio<1>> momentum_type;

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
