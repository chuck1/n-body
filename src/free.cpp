#include <cstdio>

#include <free.hpp>

void print(glm::vec3 const & v)
{
	printf("%16f%16f%16f\n", v.x, v.y, v.z);
}
void	print(float const * const v)
{
	printf("%16f%16f%16f\n", v[0], v[1], v[2]);
}

