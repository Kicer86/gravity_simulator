
#include <gmock/gmock.h>

#include "../simulation_engine.hpp"
#include "../accelerators/simple_cpu_accelerator.hpp"
#include "../accelerators/avx_accelerator.hpp"
#include "../accelerators/opencl_accelerator.hpp"


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

    std::vector<XY> forces_expected =
    {
        {3.1977106592602337e+22,  0},
        {-1.9429197577020984e+22, 0},
        {-4.8065843588514269e+21, 0},
        {-2.1147361367259475e+21, 0},
        {-1.1785555714733748e+21, 0},
        {-7.481275555246485e+20,  0},
        {-5.1596896505853641e+20, 0},
        {-3.7704938284029418e+20, 0},
        {-2.8763535564313146e+20, 0},
        {-2.2690239711976148e+20, 0},
        {-1.8391954969492822e+20, 0},
        {-1.5250834966890257e+20, 0},
        {-1.2896706684640454e+20, 0},
        {-1.109726090798039e+20,  0},
        {-9.7010368315513635e+19, 0},
        {-8.6062944454120047e+19, 0},
        {-7.7430705052729737e+19, 0},
        {-7.0625273434563674e+19, 0},
        {-6.5303584379569701e+19, 0},
        {-6.1228301705822601e+19, 0},
        {-5.8243224393434006e+19, 0},
        {-5.6260585026228322e+19, 0},
        {-5.5258270226347721e+19, 0},
        {-5.5288229719181361e+19, 0},
        {-5.6501263723501978e+19, 0},
        {-5.9202398755087188e+19, 0},
        {-6.3972511204937564e+19, 0},
        {-7.1953976062126326e+19, 0},
        {-8.5618952862731076e+19, 0},
        {-1.1131000081985674e+20, 0},
        {-1.7100750086483843e+20, 0},
        {-4.1369798775797501e+20, 0}
    };

    std::array<IAccelerator *, 3> accelerators = { &simple_cpu_accelerator, &avx_accelerator, &opencl_accelerator };

    for(IAccelerator* accelerator: accelerators)
    {
        // prepare objects
        Objects objects;

        objects.insert( Object(0, 0, 5.9736e24, 6371e3), 0);

        for(int i = 1; i < 32; i++)
            objects.insert( Object(384400e3 * i/10, 0, 7.347673e22,  1737.1e3, 0, 1.022e3 * (i%2? 1: -1)), i );

        // setup accelerator
        accelerator->setObjects(&objects);

        // verify forces correctness
        const std::vector<XY> forces = accelerator->forces();

        for(std::size_t i = 0; i < forces.size(); i++)
        {
            EXPECT_DOUBLE_EQ( forces[i].x, forces_expected[i].x );
            EXPECT_DOUBLE_EQ( forces[i].y, forces_expected[i].y );
        }

        // verify velocities for Δt = 0
        const std::vector<XY> velocities = accelerator->velocities(forces, 0);

        for(std::size_t i = 0; i < velocities.size(); i++)
        {
            EXPECT_DOUBLE_EQ( velocities[i].x, 0 );
            EXPECT_DOUBLE_EQ( velocities[i].y, 0 );
        }
    }
}
