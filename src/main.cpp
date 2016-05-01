#include <vector>

struct XY
{
    double x;
    double y;

    XY(double _x, double _y): x(_x), y(_y) {}
    XY(): x(0.0), y(0.0) {}
};

class Object
{
    XY m_pos;
    XY m_v;
    double m_mass;

    XY dF;

    public:
        Object(double x, double y, double m): m_pos(x, y), m_v(), m_mass(m) {}
};

std::vector<Object> objs;


int main(int argc, char** argv)
{
    objs.push_back( Object(0, 0, 5.9736e27) );
    objs.push_back( Object(10, 0, 1e3) );

    while(true)
    {
        for(int i = 0; i < objs.size() - 1; i++)
            for(int j = i + 1; j < objs.size(); j++)
            {
            }
    };

    return 0;
}
