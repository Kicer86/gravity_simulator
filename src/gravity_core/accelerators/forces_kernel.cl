

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
    const float G = 6.6732e-11;
    const int count_local = ((count + LOCAL_MEM_SIZE - 1) / LOCAL_MEM_SIZE) * LOCAL_MEM_SIZE;

    local float sx[LOCAL_MEM_SIZE];
    local float sy[LOCAL_MEM_SIZE];
    local float sm[LOCAL_MEM_SIZE];

    const float xi = objX[get_global_id(0)];
    const float yi = objY[get_global_id(0)];
    const float mi = mass[get_global_id(0)];

    float fx = 0, fy = 0;

    for (int c = 0; c < count_local; c += LOCAL_MEM_SIZE)
    {
      const int n = min(count - c, LOCAL_MEM_SIZE);

      for(int k = get_local_id(0); k < n; k += get_local_size(0))
      {
        sx[k] = objX[c + k];
        sy[k] = objY[c + k];
        sm[k] = mass[c + k];
      }

      barrier(CLK_LOCAL_MEM_FENCE);

      for(int k = 0; k < n; ++k)
      {
        const float xk = sx[k];
        const float yk = sy[k];
        const float mk = sm[k];
        const float dx = xk - xi;
        const float dy = yk - yi;
        float len2 = dx * dx + dy * dy;
        const int notzero = (len2 != 0);
        len2 += (len2 == 0);
        const float Fg = (G * mi) * (mk / len2);
        const float len = sqrt(len2);
        fx += dx * Fg / len * notzero;
        fy += dy * Fg / len * notzero;
      }

      barrier(CLK_LOCAL_MEM_FENCE);
    }

  if (get_global_id(0) < count)
  {
    force[get_global_id(0)] = (float2)(fx, fy);
  }
}
