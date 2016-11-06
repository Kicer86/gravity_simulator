
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

    std::vector<XY> velocities1_expected =
    {
        {0.0053530721925199032, 0},
        {-0.26442655920982361, 0},
        {-0.065416432917118073, 0},
        {-0.02878103218972683, 0},
        {-0.016039855778217316, 0},
        {-0.010181829333305359, 0},
        {-0.0070222076028585434, 0},
        {-0.005131550133228302, 0},
        {-0.0039146468043327332, 0},
        {-0.0030880849808454514, 0},
        {-0.0025030984543263912, 0},
        {-0.0020755995064973831, 0},
        {-0.0017552104545757174, 0},
        {-0.0015103094046935439, 0},
        {-0.0013202870031818748, 0},
        {-0.0011712947161868215, 0},
        {-0.0010538131464272738, 0},
        {-0.00096119183581322432, 0},
        {-0.00088876468362286687, 0},
        {-0.00083330151392146945, 0},
        {-0.00079267506953328848, 0},
        {-0.0007656922098249197, 0},
        {-0.00075205130269750953, 0},
        {-0.00075245898915454745, 0},
        {-0.00076896796235814691, 0},
        {-0.00080573000013828278, 0},
        {-0.00087064935360103846, 0},
        {-0.00097927649039775133, 0},
        {-0.0011652533430606127, 0},
        {-0.0015149025712162256, 0},
        {-0.0023273697588592768, 0},
        {-0.0056303269229829311, 0}
    };

    std::array<IAccelerator *, 3> accelerators = { &simple_cpu_accelerator, &avx_accelerator };

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
        const std::vector<XY> velocities0 = accelerator->velocities(forces, 0);

        for(std::size_t i = 0; i < velocities0.size(); i++)
        {
            EXPECT_DOUBLE_EQ( velocities0[i].x, 0 );
            EXPECT_DOUBLE_EQ( velocities0[i].y, 0 );
        }

        // verify velocities for Δt = 1
        const std::vector<XY> velocities1 = accelerator->velocities(forces, 1);

        for(std::size_t i = 0; i < velocities1.size(); i++)
        {
            EXPECT_DOUBLE_EQ( velocities1[i].x, velocities1_expected[i].x );
            EXPECT_DOUBLE_EQ( velocities1[i].y, velocities1_expected[i].y );
        }
    }
}
