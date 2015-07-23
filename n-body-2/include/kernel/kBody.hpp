#ifndef K_BODY_H
#define K_BODY_H

struct kBody
{
	float		x[3];		// 12  12
	float		v[3];		// 12  24
	float		f[3];		// 12  24

	float		mass;		//  4  28
	float		radius;		//  4  32

	unsigned char	alive;		//  1  33
	unsigned char	num_collisions;	//  1  34
};

#endif

