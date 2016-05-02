#include "simulation_engine.hpp"



int main(int argc, char** argv)
{
    SimulationEngine engine;

    engine.addObject( Object(0, 0, 5.9736e24) );
    engine.addObject( Object(6373e3, 0, 1e0, 0.0, 19000) );

    for(int i = 0; i < 1000; i++)
    {
        engine.stepBy(60);

        const std::vector<Object>& objs = engine.objects();

        for(int i = 1; i < objs.size(); i++)
        {
            const Object& o = objs[i];

            std::cout << i << "; " << o.pos().x << "; " << o.pos().y << "; " << o.velocity().x << "; " << o.velocity().y << std::endl;
        }
    }

    return 0;
}
