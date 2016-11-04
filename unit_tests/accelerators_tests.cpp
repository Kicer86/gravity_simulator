
#include <gmock/gmock.h>

#include "simulation_engine.hpp"
#include "accelerators/simple_cpu_accelerator.hpp"
#include "accelerators/avx_accelerator.hpp"
#include "accelerators/opencl_accelerator.hpp"


/*
class AcceleratorsTest: public testing::Test
{
    public:
        AcceleratorsTest():
            testing::Test()

        {
            m_engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );

            for(int i = 1; i < 32; i++)
                m_engine.addObject( Object(384400e3 * i/10, 0, 7.347673e22,  1737.1e3, 0, 1.022e3 * (i%2? 1: -1)) );
        }

    private:
        SimulationEngine m_engine;
};
*/


TEST(AcceleratorsCalculationsTest, Scenario_32SatellitesAroundBigOne)
{
    SimpleCpuAccelerator simple_cpu_accelerator;
    AVXAccelerator avx_accelerator;
    OpenCLAccelerator opencl_accelerator;

    std::array<IAccelerator *, 3> accelerators = { &simple_cpu_accelerator, &avx_accelerator, &opencl_accelerator };

    for(IAccelerator* accelerator: accelerators)
    {
        SimulationEngine engine(accelerator);

        engine.addObject( Object(0, 0, 5.9736e24, 6371e3) );

        for(int i = 1; i < 32; i++)
            engine.addObject( Object(384400e3 * i/10, 0, 7.347673e22,  1737.1e3, 0, 1.022e3 * (i%2? 1: -1)) );
    }
};
