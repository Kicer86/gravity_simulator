
#include <cmath>
#include <iostream>
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

        double mass() const
        {
            return m_mass;
        }

        const XY& pos() const
        {
            return m_pos;
        }

        void addForce(const XY &)
        {
        }
};


double distance(const Object& o1, const Object& o2)
{
    const XY& p1 = o1.pos();
    const XY& p2 = o2.pos();
    const double dist = sqrt( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );

    return dist;
}


std::vector<Object> objs;


int main(int argc, char** argv)
{
    const double G = 6.6732e-11;

    objs.push_back( Object(0, 0, 5.9736e24) );
    objs.push_back( Object(6373e3, 0, 1) );

    while(true)
    {
        for(int i = 0; i < objs.size() - 1; i++)
            for(int j = i + 1; j < objs.size(); j++)
            {
                const double dist = distance(objs[i], objs[j]);
                const double dist2 = dist * dist;
                const double masses = objs[i].mass() * objs[j].mass();
                const double Fg = G * masses / dist2;

                std::cout << Fg << std::endl;
            }
    };

    return 0;
}
