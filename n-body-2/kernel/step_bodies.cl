
#include "include/kernel/kBody.hpp"
#include "include/kernel.hpp"

bool	feq(float const * f0, float const * f1)
{
	return (fabs(*f0 - *f1) < 0.0001);
}

__kernel void step_bodies_local_indices(
		unsigned int s,
		__local unsigned int * i_local0,
		__local unsigned int * i_local1)
{
	/* work group */
	int local_block = s / get_num_groups(0);

	unsigned int i_group0 = get_group_id(0) * local_block;
	unsigned int i_group1 = i_group0 + local_block;

	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = s;

	/* work item */
	int block = (i_group1 - i_group0) / get_local_size(0);

	*i_local0 = i_group0 + get_local_id(0) * block;
	*i_local1 = *i_local0 + block;

	if(get_local_id(0) == (get_local_size(0) - 1)) *i_local1 = i_group1;
}

__kernel void step_bodies(
		__global struct kBody * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		float dt,
		unsigned int num_bodies // in
//		__global float * velocity_ratio, // in/out
//		__global float * mass_center, // in
//		float mass, // in
//		__global unsigned int * number_escaped // out
		)
{
	__local unsigned int i_local0;
	__local unsigned int i_local1;
	/*
	// work group
	int local_block = num_bodies / get_num_groups(0);

	unsigned int i_group0 = get_group_id(0) * local_block;
	unsigned int i_group1 = i_group0 + local_block;

	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = num_bodies;

	// work item
	int block = (i_group1 - i_group0) / get_local_size(0);

	i_local0 = i_group0 + get_local_id(0) * block;
	i_local1 = i_local0 + block;

	if(get_local_id(0) == (get_local_size(0) - 1)) i_local1 = i_group1;

	*/
	__local unsigned int pi0[1];
	__local unsigned int pi1[1];

	step_bodies_local_indices(num_bodies, pi0, pi1);

	i_local0 = *pi0;
	i_local1 = *pi1;

	/*
	   printf("local_block = %i\n", local_block);
	   printf("block = %i\n", block);
	   */
	/*
	   printf("i_local0 = %i\n", i_local0);
	   printf("i_local1 = %i\n", i_local1);
	   */
	/* copy data for work group */
	//__local struct Pair local_pairs[NUM_PAIRS];
	//__local struct kBodyMap local_bodymaps[NUM_BODIES / NUM_GROUPS];

	//event_t e0 = async_work_group_copy((__local char *)local_pairs, (char *)pairs, NUM_PAIRS * sizeof(struct Pair), 0);
	//wait_group_events(1, &e0);

	//event_t e1 = async_work_group_copy((__local char *)local_bodymaps, (char *)(bodymaps + i_group0), (i_group1 - i_group0) * sizeof(struct kBodyMap), 0);
	//wait_group_events(1, &e1);

	//__local struct kBodyMap * pbm = 0;
	//struct kBodyMap * pbm = 0;

	__global struct kBody * pb = 0;

	for(unsigned int b = *pi0; b < i_local1; b++)
	{
		//pbm = local_bodymaps + b;
		//pbm = bodymaps + b;

		pb = bodies + b;

		if(pb->alive == 0)
		{
			//puts("body dead");
			continue;
		}

		float dv[3];

		dv[0] = dt * pb->f[0] / pb->mass;
		dv[1] = dt * pb->f[1] / pb->mass;
		dv[2] = dt * pb->f[2] / pb->mass;


		//float e = 0.01;

		if(0)
		{
			/*
			float rat[3];
			rat[0] = fabs(dv[0] / pb->v[0]);
			rat[1] = fabs(dv[1] / pb->v[1]);
			rat[2] = fabs(dv[2] / pb->v[2]);
			*/
			// atomic
			/*
			if(std::isfinite(rat[0])) if(rat[0] > velocity_ratio[0]) velocity_ratio[0] = rat[0];
			if(std::isfinite(rat[1])) if(rat[1] > velocity_ratio[1]) velocity_ratio[1] = rat[1];
			if(std::isfinite(rat[2])) if(rat[2] > velocity_ratio[2]) velocity_ratio[2] = rat[2];
			*/

			/*
			if(
					((std::isfinite(rat[0])) && (rat[0] > e)) ||
					((std::isfinite(rat[1])) && (rat[1] > e)) ||
					((std::isfinite(rat[2])) && (rat[2] > e))
			  )
			{
				printf("% 12f % 12f % 12f\n",
						rat[0],
						rat[1],
						rat[2]);
			}
			*/
		}

		pb->v[0] += dv[0];
		pb->v[1] += dv[1];
		pb->v[2] += dv[2];

		pb->x[0] += dt * pb->v[0];
		pb->x[1] += dt * pb->v[1];
		pb->x[2] += dt * pb->v[2];

		/*
		// distance from mass center
		float r[3];
		r[0] = pb->x[0] - mass_center[0];
		r[1] = pb->x[1] - mass_center[1];
		r[2] = pb->x[2] - mass_center[2];
		
		float d = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
		
		float escape_speed2 = 2.0 * 6.67e-11 * mass / d;
		// body's absolute speed
		float s2 = pb->v[0]*pb->v[0] + pb->v[1]*pb->v[1] + pb->v[2]*pb->v[2];
		
		// dot product of velocity and displacement vector
		float dot = pb->v[0] * r[0] + pb->v[1] * r[1] + pb->v[2] * r[2];
		
		if(s2 > (escape_speed2)) // speed exceeds escape speed
		{
			if(dot > 0.0) // parallel componenet points away from mass_center
			{
				// atomic
				//(*number_escaped)++;
				//printf("escape!\n");
			}
		}
		*/
	}
}







