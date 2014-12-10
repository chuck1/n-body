#include <cmath>
#include <cstdio>
#include <cassert>

#include <Body.hpp>
#include <kernel.hpp>
#include <free.hpp>



void			reset_bodies(
		struct Body * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		float dt,
		unsigned int num_bodies, // in
		float * velocity_ratio, // in/out
		float * mass_center, // in
		float mass, // in
		unsigned int * number_escaped // out
		)
{
	/* work group */
	int local_block = num_bodies / get_num_groups(0);

	unsigned int i_group0 = get_group_id(0) * local_block;
	unsigned int i_group1 = i_group0 + local_block;

	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = num_bodies;

	/* work item */
	int block = (i_group1 - i_group0) / get_local_size(0);

	unsigned int i_local0 = i_group0 + get_local_id(0) * block;
	unsigned int i_local1 = i_local0 + block;

	if(get_local_id(0) == (get_local_size(0) - 1)) i_local1 = i_group1;

	Body * pb = 0;

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







