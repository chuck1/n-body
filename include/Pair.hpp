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
	Pair();
	Pair(Pair const & p);
	Pair &		operator=(Pair const & p);

	unsigned int	b0; // 4
	unsigned int	b1; // 4

	//float		u[3]; // 12
	//float		d; // 4
	//float		f; // 4

	// 28

	unsigned char	_M_alive; // 4
	unsigned char	_M_collision; // 4

	// 36

};

#endif
