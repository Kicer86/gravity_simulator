/*
 * AVX based accelerator for base calculations.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "avx_accelerator.hpp"

#include <immintrin.h>

#include "../objects.hpp"


namespace utils
{
    __m256d distance(const __m256d& x1, const __m256d& y1, const __m256d& x2, const __m256d& y2)
    {
        const __m256d x_diff = _mm256_sub_pd(x1, x2);
        const __m256d y_diff = _mm256_sub_pd(y1, y2);
        const __m256d x_diff2 = _mm256_mul_pd(x_diff, x_diff);
        const __m256d y_diff2 = _mm256_mul_pd(y_diff, y_diff);
        const __m256d sum = _mm256_add_pd(x_diff2, y_diff2);
        const __m256d dist = _mm256_sqrt_pd(sum);

        return dist;
    }

    struct vector
    {
        __m256d x;
        __m256d y;
    };

    vector unit_vector(const __m256d& x1, const __m256d& y1, const __m256d& x2, const __m256d& y2)
    {
        const __m256d x_diff = _mm256_sub_pd(x2, x1);
        const __m256d y_diff = _mm256_sub_pd(y2, y1);

        const __m256d dist = distance(x1, y1, x2, y2);

        const __m256d ux = _mm256_div_pd(x_diff, dist);
        const __m256d uy = _mm256_div_pd(y_diff, dist);

        vector result = {ux, uy};

        return result;
    }
}



AVXAccelerator::AVXAccelerator(Objects& objects): MTAcceleratorBase(objects)
{

}


AVXAccelerator::~AVXAccelerator()
{

}


void AVXAccelerator::forcesFor(std::size_t i, std::vector<XY>& forces) const
{
    const std::size_t objs = m_objects.size();

    // AVX can be used for 4 element aligned packs.
    const std::size_t first_simd_idx = (i + 4) & (-4);
    const std::size_t last_simd_idx = objs & (-4);

    // pre AVX calculations (for elements before first_simd_idx)
    std::size_t j = i + 1;
    for(; j < std::min(first_simd_idx, objs); j++)
    {
        const XY force_vector = force(i, j);
        const int tid = 0;

        forces[i] += force_vector;
        forces[j] += -force_vector;
    }

    // AVX calculations (for elements between first_simd_idx and last_simd_idx)
    for(; j < last_simd_idx; j+=4)
    {
        const double G = 6.6732e-11;

        const double xi    = m_objects.getX()[i];
        const __m256d x0   = {xi, xi, xi, xi};
        const __m256d x1234 = _mm256_load_pd( &m_objects.getX()[j] );

        const double yi    = m_objects.getY()[i];
        const __m256d y0   = {yi, yi, yi, yi};
        const __m256d y1234 = _mm256_load_pd( &m_objects.getY()[j] );

        const double mi    = m_objects.getMass()[i];
        const __m256d m0   = {mi, mi, mi, mi};
        const __m256d m1234 = _mm256_load_pd( &m_objects.getMass()[j] );

        const __m256d dist = utils::distance(x0, y0, x1234, y1234);
        const __m256d dist2 = _mm256_mul_pd(dist, dist);
        const __m256d masses = _mm256_mul_pd(m0, m1234);

        const __m256d vG = {G, G, G, G};
        const __m256d G_masses = _mm256_mul_pd(vG, masses);
        const __m256d Fg = _mm256_div_pd(G_masses, dist2);

        utils::vector force_vector = utils::unit_vector(x0, y0, x1234, y1234);

        force_vector.x = _mm256_mul_pd(force_vector.x, Fg);
        force_vector.y = _mm256_mul_pd(force_vector.y, Fg);

        const int tid = 0;
        for (int k = 0; k < 4; k++)
        {
            forces[i] += XY(force_vector.x[k], force_vector.y[k]);
            forces[j + k] += XY(-force_vector.x[k], -force_vector.y[k]);
        }
    }

    // post AVX calculations (for elements after last_simd_idx)
    for(; j < objs; j++)
    {
        const XY force_vector = force(i, j);
        const int tid = 0;

        forces[i] += force_vector;
        forces[j] += -force_vector;
    }
}


