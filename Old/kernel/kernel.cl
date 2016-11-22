#include "body.h"

__kernel void clear_bodies_num_collisions(
		    __global struct Body * bodies
		    )
{
	/* global index */
	
	int block = NUM_BODIES / get_global_size(0);
	
	int b0 = get_global_id(0) * block;
	int b1 = b0 + block;
	
	if(get_global_id(0) == (get_global_size(0) - 1)) b1 = NUM_BODIES;
	
	//for(int b = b_local0; b < b_local1; b++)
	for(int b = b0; b < b1; b++)
	{
		bodies[b].num_collisions = 0;
	}
}
/* dont use marcos here for global_size etc. */
__kernel void step_collisions(
		    __global struct Body* bodies, /* readonly */
		    __global struct Pair* pairs,
		    __global int * flag_multi_coll
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

	/* */
	for(int p = i_local0; p < i_local1; p++)
	{
		__global struct Pair* pp = pairs + p;

		if(!pp->alive) continue;

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

		/* modifying a body involved in multiple collisions could cause data race */
		if(b0->num_collisions > 1)
		{
			atomic_add(flag_multi_coll, 1);
			continue;
		}
		if(b1->num_collisions > 1)
		{
			atomic_add(flag_multi_coll, 1);
			continue;
		}

		__global float * x0 = bodies[pp->b0].x;
		__global float * x1 = bodies[pp->b1].x;

		if(pp->collision)
		{
			//if(b0 < b1)
			{
				//printf("collision\n");
				//num_collision++;
				
				//pairs.push_back(Pair(me,i));
				
				// total momentum
				float mom[3];
				mom[0] = b0->v[0] * b0->mass + b1->v[0] * b1->mass;
				mom[1] = b0->v[1] * b0->mass + b1->v[1] * b1->mass;
				mom[2] = b0->v[2] * b0->mass + b1->v[2] * b1->mass;

				// total mass
				float m = b0->mass + b1->mass;
	
				// mass-weighted average position
				float x[3];
				x[0] = (x0[0] * b0->mass + x1[0] * b1->mass) / m;
				x[1] = (x0[1] * b0->mass + x1[1] * b1->mass) / m;
				x[2] = (x0[2] * b0->mass + x1[2] * b1->mass) / m;
	
				x0[0] = x[0];
				x0[1] = x[1];
				x0[2] = x[2];
	
				// absorb mass
				b0->mass = m;
				b0->radius = pow(3.0 / 4.0 / 3.1415 * m / 900.0, 0.333333);
		
				/*
				printf("mass = %f radius = %f\n", m, u.r(t, me));
				printf("p = %f %f %f\n",
						u.p(t, me).x,
						u.p(t, me).y,
						u.p(t, me).z);
				*/
	
				// new velocity
				b0->v[0] = mom[0] / m;
				b0->v[1] = mom[1] / m;
				b0->v[2] = mom[2] / m;
	
				b1->alive = 0;
				pp->alive = 0;
			}

		}

	
		float r[3];
		
		r[0] = x0[0] - x1[0];
		r[1] = x0[1] - x1[1];
		r[2] = x0[2] - x1[2];
	
		float d2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];

		float dr = rsqrt(d2);

		pp->d = sqrt(d2);
		
		pp->u[0] = r[0] * dr;
		pp->u[1] = r[1] * dr;
		pp->u[2] = r[2] * dr;
		
		pp->f = 6.67384E-11 * b0->mass * b1->mass / d2;
	}
}
