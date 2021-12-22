//
// Created by vkoskiv on 12/19/21.
//

#pragma once

#include <limits>

typedef double real;
constexpr static real ray_eps = 1e-5; //1e-9;
constexpr static real ray_inf = std::numeric_limits<real>::infinity();

constexpr static real real_pi  = 3.1415926535897932384626433832795;
constexpr static real real_2pi = 6.283185307179586476925286766559;
constexpr static real real_inv_pi  = 0.31830988618379067153776752674503;
constexpr static real real_inv_2pi = 0.15915494309189533576888376337251;
