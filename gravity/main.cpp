#include "ocl_kernel.hpp"
#include "timer.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>

#include <cstdlib>

void cpu_forces(const float *objX, const float *objY, const float *mass,
                float *forcex, float *forcey, const int count) {
  const float G = 6.6732e-11;
  for (int i = 0; i < count; ++i) {
    float fx = 0, fy = 0;
    float xi = objX[i];
    float yi = objY[i];
    float mi = mass[i];
    for (int j = 0; j < count; j++) {
      if (i == j)
        continue;

      float xj = objX[j];
      float yj = objY[j];
      float mj = mass[j];

      float dx = xj - xi;
      float dy = yj - yi;

      float len2 = dx * dx + dy * dy;
      const float Fg = (G * mi) * (mj / len2);
      float len = sqrt(len2);

      fx += dx / len * Fg;
      fy += dy / len * Fg;
    }
    forcex[i] = fx;
    forcey[i] = fy;
  }
}

template <typename T1, typename T2>
bool compare(const T1 &t1, const T2 &t2, float eps = 1e-8) {
  return std::equal(t1.begin(), t1.end(), t2.begin(),
                    [=](float f1, float f2) { return fabs(f1 - f2) < eps; });
}

void assert(bool v, const char * msg) {
  if(!v) {
    std::cout << "Assertion failed: " << msg << std::endl;
    abort();
  }
}

int main(int argc, char **argv) {

  if(argc != 2) {
    std::cout << argv[0] << " <problem-size>" << std::endl;
  }

  /*
   * Tuning parameter. Problem size.
   */
  const size_t siz = atoi(argv[1]);

  std::vector<float> x(siz);
  std::vector<float> y(siz);
  std::vector<float> m(siz);

  std::vector<float> f1x(siz);
  std::vector<float> f1y(siz);

  std::vector<float> f2x(siz);
  std::vector<float> f2y(siz);

  std::generate(x.begin(), x.end(),
                []() { return (float)rand() / (float)RAND_MAX; });
  std::generate(y.begin(), y.end(),
                []() { return (float)rand() / (float)RAND_MAX; });
  std::generate(m.begin(), m.end(),
                []() { return (float)rand() / (float)RAND_MAX; });

  {
    Timer t;
    cpu_forces(&x[0], &y[0], &m[0], &f1x[0], &f1y[0], siz);
  }

  {
    OpenCL opencl;
    std::fill(f2x.begin(), f2x.end(), 0);
    std::fill(f2y.begin(), f2y.end(), 0);
    {
      Timer t;
      opencl.exec1(&x[0], &y[0], &m[0], &f2x[0], &f2y[0], siz);
    }
    assert (compare(f1x, f2x), "results differ too much");
    assert (compare(f1y, f2y), "results differ too much");
  }
  {
    OpenCL opencl;
    std::fill(f2x.begin(), f2x.end(), 0);
    std::fill(f2y.begin(), f2y.end(), 0);
    {
      Timer t;
      opencl.exec2(&x[0], &y[0], &m[0], &f2x[0], &f2y[0], siz);
    }
    assert (compare(f1x, f2x), "results differ too much");
    assert (compare(f1y, f2y), "results differ too much");
  }
  return 0;
}
