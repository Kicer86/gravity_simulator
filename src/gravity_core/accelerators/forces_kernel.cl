
struct XY
{
    float x;
    float y;
};


float point_distance(float x1, float y1, float x2, float y2)
{
    const float dist = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );

    return dist;
}


struct XY unit_vector(float x1, float y1, float x2, float y2)
{
    struct XY v;
    v.x = x2 - x1;
    v.y = y2 - y1;

    const float dist = point_distance(x1, y1, x2, y2);

    v.x /= dist;
    v.y /= dist;

    return v;
}


__kernel void forces(__global const float* objX,
                        __global const float* objY,
                        __global const float* mass,
                        __global float* forceX,
                        __global float* forceY,
                        const int count
                    )
{
    const int i = get_global_id(0);
    const float G = 6.6732e-11;

    if (i < count)
    {
        forceX[i] = 0;
        forceY[i] = 0;

        for(int j = 0; j < count; j++)
        {
            if (i == j)
                continue;

            const float x1 = objX[i];
            const float y1 = objY[i];
            const float x2 = objX[j];
            const float y2 = objY[j];
            const float m1 = mass[i];
            const float m2 = mass[j];

            const float dist = point_distance(x1, y1, x2, y2);
            const float dist2 = dist * dist;
            const float Fg = (G * m1) * (m2 / dist2);

            struct XY force_vector = unit_vector(x1, y1, x2, y2);
            force_vector.x *= Fg;
            force_vector.y *= Fg;

            forceX[i] += force_vector.x;
            forceY[i] += force_vector.y;
        }
    }
}
