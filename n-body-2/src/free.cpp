#include <cstdio>

#include <free.hpp>

float radius(float m)
{
	return pow(m / 900.0 * 3.0 / 4.0 / M_PI, 0.3333333333);
}

void print(glm::vec3 const & v)
{
	printf("%16e%16e%16e\n", v.x, v.y, v.z);
}
void	print(float const * const v)
{
	printf("%16f%16f%16f\n", v[0], v[1], v[2]);
}

unsigned int	map_func__(unsigned int i, unsigned int j, unsigned int n)
{
	return ((i+1) / 2 * (2*n - 2 - i)) - (n - i - 1) + (j - i - 1);
}
unsigned int	map_func(unsigned int i, unsigned int j, unsigned int n)
{
	if(i > j) return map_func__(j, i, n);
	return map_func__(i, j, n);
}

