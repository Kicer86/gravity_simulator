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
    __m256 distance(const __m256& x1, const __m256& y1, const __m256& x2, const __m256& y2)
    {
        const __m256 x_diff = _mm256_sub_ps(x1, x2);
        const __m256 y_diff = _mm256_sub_ps(y1, y2);
        const __m256 x_diff2 = _mm256_mul_ps(x_diff, x_diff);
        const __m256 y_diff2 = _mm256_mul_ps(y_diff, y_diff);
        const __m256 sum = _mm256_add_ps(x_diff2, y_diff2);
        const __m256 dist = _mm256_sqrt_ps(sum);

        return dist;
    }

    struct vector
    {
        __m256 x;
        __m256 y;
    };

    vector unit_vector(const __m256& x1, const __m256& y1, const __m256& x2, const __m256& y2)
    {
        const __m256 x_diff = _mm256_sub_ps(x2, x1);
        const __m256 y_diff = _mm256_sub_ps(y2, y1);

        const __m256 dist = distance(x1, y1, x2, y2);

        const __m256 ux = _mm256_div_ps(x_diff, dist);
        const __m256 uy = _mm256_div_ps(y_diff, dist);

        vector result = {ux, uy};

        return result;
    }
}



AVXAccelerator::AVXAccelerator(Objects& objects): CpuAcceleratorBase (objects)
{

}


AVXAccelerator::~AVXAccelerator()
{

}


void AVXAccelerator::forcesFor(std::size_t i, std::vector<XY>& forces) const
{
    const std::size_t objs = m_objects.size();

    // AVX can be used for 8 element aligned packs.
    const std::size_t first_simd_idx = (i + 8) & (-8);
    const std::size_t last_simd_idx = objs & (-8);

    // pre AVX calculations (for elements before first_simd_idx)
    std::size_t j = i + 1;
    for(; j < std::min(first_simd_idx, objs); j++)
    {
        const XY force_vector = force(i, j);

        forces[i] += force_vector;
        forces[j] += -force_vector;
    }

    // AVX calculations (for elements between first_simd_idx and last_simd_idx)
    for(; j < last_simd_idx; j+=8)
    {
        const float G = 6.6732e-11;

        const float xi    = m_objects.getX()[i];
        const __m256 x0   = {xi, xi, xi, xi, xi, xi, xi, xi};
        const __m256 x1234 = _mm256_load_ps( &m_objects.getX()[j] );

        const float yi    = m_objects.getY()[i];
        const __m256 y0   = {yi, yi, yi, yi, yi, yi, yi, yi};
        const __m256 y1234 = _mm256_load_ps( &m_objects.getY()[j] );

        const float mi    = m_objects.getMass()[i];
        const __m256 m0   = {mi, mi, mi, mi, mi, mi, mi, mi};
        const __m256 m1234 = _mm256_load_ps( &m_objects.getMass()[j] );

        const __m256 dist = utils::distance(x0, y0, x1234, y1234);
        const __m256 dist2 = _mm256_mul_ps(dist, dist);

        const __m256 vG = {G, G, G, G, G, G, G, G};
        const __m256 vG_m0 = _mm256_mul_ps(vG, m0);

        const __m256 m1234_dist2 = _mm256_div_ps(m1234, dist2);

        const __m256 Fg = _mm256_mul_ps(vG_m0, m1234_dist2);

        utils::vector force_vector = utils::unit_vector(x0, y0, x1234, y1234);

        force_vector.x = _mm256_mul_ps(force_vector.x, Fg);
        force_vector.y = _mm256_mul_ps(force_vector.y, Fg);

        for (int k = 0; k < 8; k++)
        {
            forces[i] += XY(force_vector.x[k], force_vector.y[k]);
            forces[j + k] += XY(-force_vector.x[k], -force_vector.y[k]);
        }
    }

    // post AVX calculations (for elements after last_simd_idx)
    for(; j < objs; j++)
    {
        const XY force_vector = force(i, j);

        forces[i] += force_vector;
        forces[j] += -force_vector;
    }
}
