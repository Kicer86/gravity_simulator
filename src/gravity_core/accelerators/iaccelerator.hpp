
#ifndef IACCELERATOR_HPP
#define IACCELERATOR_HPP

#include <vector>

#include "../object.hpp"

class Objects;

struct IAccelerator
{
    virtual ~IAccelerator() = default;

    virtual void setObjects(Objects *) = 0;

    virtual std::vector<XY> forces() = 0;
    virtual std::vector<XY> velocities(const std::vector<XY>& forces, double dt) const = 0;
    virtual std::vector< std::pair<int, int> > collisions() const = 0;
};


#endif
