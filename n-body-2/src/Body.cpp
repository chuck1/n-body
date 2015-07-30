#include <cstdio>

#include <Body.hpp>

Body::Body():
	alive(1),
	mass(1),
	radius(1)
{
}
void	Body::print()
{
	printf("position = %12.2e%12.2e%12.2e%12.2e%12.2e%12.2e%12.2e%12.2e\n", x.x, x.y, x.z, f.x, f.y, f.z, radius, mass);
}

