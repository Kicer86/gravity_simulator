#include "ocl_kernel.hpp"
#include "timer.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>

#include <cstdlib>

template <typename T> bool cmpf(T, T, int);

template <> bool cmpf<float>(float a, float b, int max_diff) {
  static_assert(sizeof(float) == sizeof(int), "Invalid size");
  int ai, bi;
  memcpy(&ai, &a, sizeof(float));
  memcpy(&bi, &b, sizeof(float));
  if (ai < 0)
    ai = 0x80000000 - ai;
  if (bi < 0)
    bi = 0x80000000 - bi;
  return (abs(ai - bi) <= max_diff);
}

void cpu_forces(const float *__restrict__ objX, const float *__restrict__ objY,
                const float *__restrict__ mass, float *__restrict__ forcex,
                float *__restrict__ forcey, const int count) {
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
      if (len2 == 0)
        continue;

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
bool compare(const T1 &t1, const T2 &t2, int max_diff = 4) {
  int i = 0;
  return std::equal(t1.begin(), t1.end(), t2.begin(), [&](float f1, float f2) {
    ++i;
    if (!cmpf(f1, f2, max_diff)) {
      std::cout << i << ": " << f1 << " != " << f2 << " " << (f1 - f2)
                << std::endl;
      return false;
    } else
      return true;
  });
}

void assert(bool v, const char *msg) {
  if (!v) {
    std::cout << "Assertion failed: " << msg << std::endl;
    //    abort();
  }
}

int main(int argc, char **argv) {

  if (argc != 2 || !isdigit(argv[1][0])) {
    std::cout << argv[0] << " <problem-size>" << std::endl;
    return 1;
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

  auto gen = []() { return (float)rand() / (float)RAND_MAX; };
  std::generate(x.begin(), x.end(), gen);
  std::generate(y.begin(), y.end(), gen);
  std::generate(m.begin(), m.end(), gen);

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
    assert(compare(f1x, f2x), "results differ too much");
    assert(compare(f1y, f2y), "results differ too much");
  }
  {
    OpenCL opencl;
    std::fill(f2x.begin(), f2x.end(), 0);
    std::fill(f2y.begin(), f2y.end(), 0);
    {
      Timer t;
      opencl.exec2(&x[0], &y[0], &m[0], &f2x[0], &f2y[0], siz);
    }
    assert(compare(f1x, f2x), "results differ too much");
    assert(compare(f1y, f2y), "results differ too much");
  }
  {
    OpenCL opencl;
    std::fill(f2x.begin(), f2x.end(), 0);
    std::fill(f2y.begin(), f2y.end(), 0);
    {
      Timer t;
      opencl.exec3(&x[0], &y[0], &m[0], &f2x[0], &f2y[0], siz);
    }
    assert(compare(f1x, f2x), "results differ too much");
    assert(compare(f1y, f2y), "results differ too much");
  }

  return 0;
}
