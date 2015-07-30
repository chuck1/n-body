#ifndef BODY_HPP
#define BODY_HPP

#include <glm/glm.hpp>

#include <kernel/kBody.hpp>

class Body
{
public:
	Body();
	void	print();
	
	glm::vec3	x;		// 12  12
	glm::vec3	v;		// 12  24
	glm::vec3	f;		// 12  24
	
	float		mass;		//  4  28
	float		radius;		//  4  32

	unsigned char	alive;		//  1  33
	unsigned char	num_collisions;	//  1  34
};

#endif

