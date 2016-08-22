/*
 * Container for Objects
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

#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <cstdlib>
#include <limits>
#include <vector>

#include "object.hpp"
#include "types.hpp"

class Objects
{
    public:

        // based on http://www.josuttis.com/cppcode/myalloc.hpp.html
        template<typename T, int alignment>
        class AlignmentAllocator
        {
            public:
                // type definitions
                typedef T        value_type;
                typedef T*       pointer;
                typedef const T* const_pointer;
                typedef T&       reference;
                typedef const T& const_reference;
                typedef std::size_t    size_type;
                typedef std::ptrdiff_t difference_type;

                // rebind allocator to type U
                template <class U>
                struct rebind
                {
                    typedef AlignmentAllocator<U, alignment> other;
                };

                // return maximum number of elements that can be allocated
                size_type max_size() const throw()
                {
                    return std::numeric_limits<std::size_t>::max() / sizeof(T);
                }

                // return address of values
                pointer address (reference value) const
                {
                    return &value;
                }
                const_pointer address (const_reference value) const
                {
                    return &value;
                }

                pointer allocate(std::size_t num, const void* = 0)
                {
                    num += alignment - 1;
                    num &= (-alignment);

                    const std::size_t num_bytes = num * sizeof(T);

                    return static_cast<pointer>( aligned_alloc(alignment, num_bytes) );
                }

                void deallocate(T* ptr, std::size_t)
                {
                    free(ptr);
                }
        };

        typedef std::vector<BaseType, AlignmentAllocator<BaseType, 64>> DataVector;

        Objects();
        Objects(const Objects &) = delete;
        ~Objects();

        Objects& operator=(const Objects &) = delete;
        Object operator[](std::size_t idx) const;              // returns Object for given index (index ≠ Object::id)

        std::size_t size() const;

        std::size_t insert(const Object &, std::size_t id);    // returns Object's index. Index is valid until next modification of Objects
        void erase(std::size_t idx);                           // erase item at index 'idx'. Last item will overwrite 'idx' and list will shrink

        // hight level access
        void setPos(std::size_t idx, const XY &);
        void setVelocity(std::size_t idx, const XY &);
        void setMass(std::size_t idx, BaseType);
        void setRadius(std::size_t idx, BaseType);

        XY getPos(std::size_t idx) const;
        XY getVelocity(std::size_t idx) const;
        //

        // raw data access for accelerators' purposes
        const DataVector& getX() const;
        DataVector& getX();

        const DataVector& getY() const;
        DataVector& getY();

        const DataVector& getVX() const;
        DataVector& getVX();

        const DataVector& getVY() const;
        DataVector& getVY();

        const DataVector& getMass() const;
        DataVector& getMass();

        const DataVector& getRadius() const;
        DataVector& getRadius();

        const std::vector<int>& getId() const;
        std::vector<int>& getId();

    private:

        // objects data

        DataVector m_x;
        DataVector m_y;
        DataVector m_vx;
        DataVector m_vy;
        DataVector m_mass;
        DataVector m_radius;
        std::vector<int> m_id;
};

#endif // OBJECTS_HPP
