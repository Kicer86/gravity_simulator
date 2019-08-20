
#ifndef TYPES_HPP
#define TYPES_HPP

#include "../libs/si/include/SI/force.h"

typedef float BaseType;

typedef SI::newton_t<BaseType> newton_t;

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
