//
// Created by vkoskiv on 12/19/21.
//

#pragma once

#include <cmath>
#include "real.h"



template<typename real_type>
class vec3
{
public:
    union
    {
        struct { real_type x, y, z; };
        real_type xyz[3];
    };

    inline constexpr vec3<real_type>() { }
    inline constexpr vec3<real_type>(const vec3<real_type> & v) : x(v.x), y(v.y), z(v.z) { }
    inline constexpr vec3<real_type>(real_type v) : x(v), y(v), z(v) { }
    inline constexpr vec3<real_type>(real_type x_, real_type y_, real_type z_) : x(x_), y(y_), z(z_) { }

    inline constexpr real_type & operator[](int i) { return xyz[i]; }
    inline constexpr real_type operator[](int i) const { return xyz[i]; }

    inline constexpr vec3<real_type> operator+(const vec3<real_type> & rhs) const { return vec3<real_type>(x + rhs.x, y + rhs.y, z + rhs.z); }
    inline constexpr vec3<real_type> operator-(const vec3<real_type> & rhs) const { return vec3<real_type>(x - rhs.x, y - rhs.y, z - rhs.z); }
    inline constexpr vec3<real_type> operator*(const real_type rhs) const { return vec3<real_type>(x * rhs, y * rhs, z * rhs); }
    inline constexpr vec3<real_type> operator/(const real_type rhs) const { return vec3<real_type>(x / rhs, y / rhs, z / rhs); }

    inline constexpr vec3<real_type> operator*(const vec3<real_type> & rhs) const { return vec3<real_type>(x * rhs.x, y * rhs.y, z * rhs.z); }
    inline constexpr vec3<real_type> operator/(const vec3<real_type> & rhs) const { return vec3<real_type>(x / rhs.x, y / rhs.y, z / rhs.z); }

    inline constexpr vec3<real_type> operator-() const { return vec3<real_type>(-x, -y, -z); }

    inline constexpr const vec3<real_type> & operator =(const real_type rhs) { x = rhs; y = rhs; z = rhs; return *this; }

    inline constexpr const vec3<real_type> & operator =(const vec3<real_type> & rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }

    inline constexpr const vec3<real_type> & operator+=(const vec3<real_type> & rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    inline constexpr const vec3<real_type> & operator-=(const vec3<real_type> & rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    inline constexpr const vec3<real_type> & operator*=(const vec3<real_type> & rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
    inline constexpr const vec3<real_type> & operator*=(const real_type rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }

    inline constexpr real_type length2() const { return x * x + y * y + z * z; }
    inline constexpr real_type length()  const { return std::sqrt(length2());  }

    inline constexpr void normalise(const real_type len = 1) { const real_type s = len / length(); x *= s; y *= s; z *= s; }
};


template<typename real_type>
inline static constexpr real_type dot(const vec3<real_type> & lhs, const vec3<real_type> & rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }


template<typename real_type>
inline static constexpr vec3<real_type> cross(const vec3<real_type> & a, const vec3<real_type> & b) { return vec3<real_type>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }


template<typename real_type>
inline static constexpr real_type length(const vec3<real_type> & v) { return v.length(); }


template<typename real_type>
inline static vec3<real_type> normalise(const vec3<real_type> & v, const real_type len = 1) { return v * (len / v.length()); }


typedef vec3<float> vec3f;
typedef vec3<double> vec3d;
typedef vec3<real> vec3r;


inline static vec3f toVec3f(const vec3d & v) { return vec3f((float)v.x, (float)v.y, (float)v.z); }
inline static vec3d toVec3d(const vec3f & v) { return vec3d(v.x, v.y, v.z); }
