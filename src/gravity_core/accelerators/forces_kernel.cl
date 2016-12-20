

float2 unit_vector(float2 p1, float2 p2)
{
    float2 v = p2 - p1;

    const float dist = length(v);

    v.x /= dist;
    v.y /= dist;

    return v;
}


kernel void forces(global const float* objX,
                   global const float* objY,
                   global const float* mass,
                   global float2* force,
                   const int count
                  )
{
    const int i = get_global_id(0);
    const float G = 6.6732e-11;

    if (i < count)
    {
        force[i] = (float2)(0, 0);

        const float2 p1 = (float2)( objX[i], objY[i] );
        const float m1 = mass[i];

        for(int j = 0; j < count; j++)
        {
            if (i == j)
                continue;

            const float2 p2 = (float2)( objX[j], objY[j] );
            const float m2 = mass[j];

            const float dist = length(p2 - p1);
            const float dist2 = dist * dist;
            const float Fg = (G * m1) * (m2 / dist2);

            float2 force_vector = unit_vector(p1, p2);
            force_vector *= Fg;

            force[i] += force_vector;
        }

    }
}
