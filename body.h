#ifndef BODY_H
#define BODY_H

#include <cstdio>
#include <cstring>
#include <cassert>

//#define NUM_BODIES (16)
//#define NUM_PAIRS (NUM_BODIES * (NUM_BODIES - 1) / 2)

#define GLOBAL_SIZE (128)
#define LOCAL_SIZE (8)
#define NUM_GROUPS (GLOBAL_SIZE / LOCAL_SIZE)


struct Body
{
	Body(): x{0,0,0}, v{0,0,0}, mass(0), radius(0), alive(1), num_collisions(0)
	{
		//printf("%s\n", __PRETTY_FUNCTION__);
	}
	Body(Body const & b): mass(b.mass), radius(b.radius), alive(b.alive), num_collisions(b.num_collisions)
	{
		memcpy(x, b.x, 12);
		memcpy(v, b.v, 12);
	}
	Body &		operator=(Body const & b)
	{
		
		memcpy(x, b.x, 12);
		memcpy(v, b.v, 12);
		mass = b.mass;
		radius = b.radius;
		alive = b.alive;
		num_collisions = b.num_collisions;
		return *this;
	}

	float	x[3]; // 4 * 3 = 12
	float	v[3]; // 4 * 3 = 12
	float	mass; // 4
	float	radius; // 4
	// 12 + 12 + 4 + 4 = 32
	
	unsigned char	alive; // 4
	unsigned char	num_collisions; // 4
	
	// 40
	
};

struct Map
{
	void			alloc(unsigned int n)
	{
		pair_ = new unsigned int[n*n];
	}
	unsigned int *		ptr()
	{
		return pair_;
	}
	unsigned int *		pair_; //[NUM_BODIES * NUM_BODIES];
};

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
