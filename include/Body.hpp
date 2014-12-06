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

#define CPU 1

#ifdef CPU
#include <glm/glm.hpp>
#endif

struct Body
{
#ifdef CPU
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
#endif
	union
	{
		float		x[3];		// 12  12
		glm::vec3	x_glm;
	};
	union
	{
		float		v[3];		// 12  24
		glm::vec3	v_glm;
	};

	float		mass;		//  4  28
	float		radius;		//  4  32

	unsigned char	alive;		//  1  33
	unsigned char	num_collisions;	//  1  34

};


#endif

