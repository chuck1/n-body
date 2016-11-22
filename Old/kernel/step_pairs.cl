#include "body.h"

__kernel void step_pairs(
		    __global struct Body * bodies,
		    __global struct Pair * pairs
		    )
{
	/* work group */
	int local_block = NUM_PAIRS / get_num_groups(0);
	
	int i_group0 = get_group_id(0) * local_block;
	int i_group1 = i_group0 + local_block;
	
	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = NUM_PAIRS;
	
	/* work item */
	int block = (i_group1 - i_group0) / get_local_size(0);
	
	int i_local0 = i_group0 + get_local_id(0) * block;
	int i_local1 = i_local0 + block;
	
	if(get_local_id(0) == (get_local_size(0) - 1)) i_local1 = i_group1;

	/* copy data for work group */
	//__local struct Body local_bodies[NUM_BODIES];

	//event_t e = async_work_group_copy((__local char *)local_bodies, (__global char *)bodies, NUM_BODIES * sizeof(struct Body), 0);
	//wait_group_events(1, &e);
	
	/* compute */
	for(int p = i_local0; p < i_local1; p++)
	{
		__global struct Pair* pp = pairs + p;

		if(!pp->alive) continue;

		//__local struct Body* b0 = local_bodies + pp->b0;
		//__local struct Body* b1 = local_bodies + pp->b1;
		__global struct Body* b0 = bodies + pp->b0;
		__global struct Body* b1 = bodies + pp->b1;

		if(!b0->alive)
		{
			pp->alive = 0;
			continue;
		}
		if(!b1->alive)
		{
			pp->alive = 0;
			continue;
		}
		
		//__local float * x0 = b0->x;
		//__local float * x1 = b1->x;
		__global float * x0 = b0->x;
		__global float * x1 = b1->x;
	
		float r[3];
		
		r[0] = x0[0] - x1[0];
		r[1] = x0[1] - x1[1];
		r[2] = x0[2] - x1[2];
	
		float d2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];

		float dr = rsqrt(d2);

		pp->d = sqrt(d2);
	
		if(pp->d < (b0->radius + b1->radius))
		{
			pp->collision = 1;
			b0->num_collisions++;
			b1->num_collisions++;
		}

		pp->u[0] = r[0] * dr;
		pp->u[1] = r[1] * dr;
		pp->u[2] = r[2] * dr;
		
		pp->f = 6.67384E-11 * b0->mass * b1->mass / d2;
	}

}
