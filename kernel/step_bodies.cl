//#include "body.h"
#include "include/Body.hpp"

__kernel void step_bodies(
		    __global struct Body * bodies,
		    __global struct Pair * pairs,
		    __global struct Map * map,
		    float dt
		   )
{
	if(sizeof(struct Body) == 40)
	{
	}

	/* work group */
	int local_block = NUM_BODIES / get_num_groups(0);
	
	int i_group0 = get_group_id(0) * local_block;
	int i_group1 = i_group0 + local_block;
	
	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = NUM_BODIES;
	
	/* work item */
	int block = (i_group1 - i_group0) / get_local_size(0);
	
	int i_local0 = i_group0 + get_local_id(0) * block;
	int i_local1 = i_local0 + block;
	
	if(get_local_id(0) == (get_local_size(0) - 1)) i_local1 = i_group1;

	/* copy data for work group */
	//__local struct Pair local_pairs[NUM_PAIRS];
	//__local struct BodyMap local_bodymaps[NUM_BODIES / NUM_GROUPS];
	
	//event_t e0 = async_work_group_copy((__local char *)local_pairs, (__global char *)pairs, NUM_PAIRS * sizeof(struct Pair), 0);
	//wait_group_events(1, &e0);

	//event_t e1 = async_work_group_copy((__local char *)local_bodymaps, (__global char *)(bodymaps + i_group0), (i_group1 - i_group0) * sizeof(struct BodyMap), 0);
	//wait_group_events(1, &e1);

	/* */
	float f[3];

	//__local struct BodyMap * pbm = 0;
	//__global struct BodyMap * pbm = 0;

	__global struct Body * pb = 0;
	
	for(int b = i_local0; b < i_local1; b++)
	{
		//pbm = local_bodymaps + b;
		//pbm = bodymaps + b;

		pb = bodies + b;
	
		if(!pb->alive) continue;

		f[0] = 0;
		f[1] = 0;
		f[2] = 0;
		
		for(int i = 0; i < NUM_BODIES; i++)
		{
			if(b == i) continue;

			float s = 1.0;
		
			//__local struct Pair * pp = &local_pairs[pbm->pair[p]];
			__global struct Pair * pp = pairs + map->pair[b * NUM_BODIES + i];
			
			if(!pp->alive) continue;

			if(pp->b0 == b)
			{
				s = - 1.0;
			}

			f[0] += pp->u[0] * pp->f * s;
			f[1] += pp->u[1] * pp->f * s;
			f[2] += pp->u[2] * pp->f * s;
		}
		
		pb->v[0] += dt * f[0] / pb->mass;
		pb->v[1] += dt * f[1] / pb->mass;
		pb->v[2] += dt * f[2] / pb->mass;

		pb->x[0] += dt * pb->v[0];
		pb->x[1] += dt * pb->v[1];
		pb->x[2] += dt * pb->v[2];
	}
}
