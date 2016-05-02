
#include <cmath>
#include <iostream>
#include <vector>


struct XY
{
    double x;
    double y;

    XY(double _x, double _y): x(_x), y(_y) {}
    XY(): x(0.0), y(0.0) {}

    XY operator-(const XY& other) const
    {
        XY result = *this;
        result.x -= other.x;
        result.y -= other.y;

        return result;
    }

    XY& operator*=(double v)
    {
        x *= v;
        y *= v;

        return *this;
    }

    XY operator*(double v) const
    {
        XY result(x, y);
        result *= v;

        return result;
    }

    XY& operator+=(const XY& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    XY operator-() const
    {
        XY result(-x, -y);

        return result;
    }

    XY& operator/(double v)
    {
        x /= v;
        y /= v;

        return *this;
    }
};


class Object
{
    XY m_pos;
    XY m_v;
    double m_mass;

    XY dF;

    public:
        Object(double x, double y, double m, double v_x = 0.0, double v_y = 0.0): m_pos(x, y), m_v(v_x, v_y), m_mass(m) {}

        double mass() const
        {
            return m_mass;
        }

        const XY& pos() const
        {
            return m_pos;
        }

        const XY& velocity() const
        {
            return m_v;
        }

        void addForce(const XY& f)
        {
            dF += f;
        }

        void applyForce(double dt)
        {
            // F=am ⇒ a = F/m
            const XY a = dF/m_mass;

            // ΔV = aΔt
            const XY dv = a * dt;

            m_v += dv;
            m_pos += m_v * dt;

            dF.x = 0.0;
            dF.y = 0.0;
        }
};


double distance(const Object& o1, const Object& o2)
{
    const XY& p1 = o1.pos();
    const XY& p2 = o2.pos();
    const double dist = sqrt( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );

    return dist;
}


XY unit_vector(const Object& o1, const Object& o2)
{
    XY v( o1.pos() - o2.pos() );
    const double dist = distance(o1, o2);

    v.x /= dist;
    v.y /= dist;

    return v;
}


int main(int argc, char** argv)
{
    std::vector<Object> objs;

    const double G = 6.6732e-11;

    objs.push_back( Object(0, 0, 5.9736e24) );
    objs.push_back( Object(6373e3, 0, 1e0, 0.0, 19000) );

    bool done = false;

    for(int l = 0; l < 1000; l++)
    {
        for(int i = 0; i < objs.size() - 1; i++)
            for(int j = i + 1; j < objs.size(); j++)
            {
                Object& o1 = objs[i];
                Object& o2 = objs[j];

                const double dist = distance(o1, o2);
                const double dist2 = dist * dist;
                const double masses = o1.mass() * o2.mass();
                const double Fg = G * masses / dist2;

                XY force_vector = unit_vector(o2, o1);
                force_vector *= Fg;

                o1.addForce(force_vector);
                o2.addForce(-force_vector);
            }

        for(int i = 0; i < objs.size(); i++)
            objs[i].applyForce(60);

        for(int i = 1; i < objs.size(); i++)
        {
            const Object& o = objs[i];

            std::cout << l << "; " << o.pos().x << "; " << o.pos().y << "; " << o.velocity().x << "; " << o.velocity().y << std::endl;
        }
    };

    return 0;
}
