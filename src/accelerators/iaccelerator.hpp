
#ifndef IACCELERATOR_HPP
#define IACCELERATOR_HPP


struct IAccelerator
{
    virtual ~IAccelerator() = default;

    virtual double step() = 0;
};


#endif
