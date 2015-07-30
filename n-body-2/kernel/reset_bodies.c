
#include "include/kernel/kBody.hpp"
#include "include/kernel/other.hpp"

__kernel void			reset_bodies(
		__global struct kBody * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		unsigned int num_bodies
		//float * velocity_ratio, // in/out
		//float * mass_center, // in
		//float mass, // in
		//unsigned int * number_escaped // out
		)
{
	unsigned int i_local0;
	unsigned int i_local1;
	
	divide(num_bodies, i_local0, i_local1);
	
	__global struct kBody * pb = 0;

	for(unsigned int b = i_local0; b < i_local1; b++)
	{

		pb = bodies + b;

		if(pb->alive == 0)
		{
			//puts("body dead");
			continue;
		}

		// reset accumulating force
		pb->f[0] = 0;
		pb->f[1] = 0;
		pb->f[2] = 0;

	}
}







