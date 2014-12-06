#ifndef PAIR_H
#define PAIR_H

#include <cstdio>
#include <cstring>
#include <cassert>

#ifdef CPU
#include <glm/glm.hpp>
#endif


struct Pair
{
	Pair(): b0(0), b1(0), u{0,0,0}, d(0), f(0), _M_alive(1), _M_collision(0) {}
	Pair(Pair const & p)
	{
		assert(0);
	}
	Pair &		operator=(Pair const & p)
	{
		assert(0);
	}

	unsigned int	b0; // 4
	unsigned int	b1; // 4

	float		u[3]; // 12
	float		d; // 4
	float		f; // 4

	// 28

	unsigned char	_M_alive; // 4
	unsigned char	_M_collision; // 4

	// 36

};

#endif
