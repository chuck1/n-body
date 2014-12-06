
#include <kernel.h>
#include <Branches.hpp>


void			update_branches(
		Branches * branches,
		unsigned int level // lowest level
		)
{
	// start at lowest level
	
	for( ;level >= 0; level--)
	{
		for(unsigned int idx = 0; idx < branches->_M_num_branches; idx++)
		{

		}
	}
}

void			step_branchpairs(
		Branches * branches
		)
{
	/* work group */
	int local_block = branches->_M_num_branch_pairs / get_num_groups(0);
	
	int i_group0 = get_group_id(0) * local_block;
	int i_group1 = i_group0 + local_block;
	
	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = branches->_M_num_branch_pairs;
	
	/* work item */
	int block = (i_group1 - i_group0) / get_local_size(0);
	
	int i_local0 = i_group0 + get_local_id(0) * block;
	int i_local1 = i_local0 + block;
	
	if(get_local_id(0) == (get_local_size(0) - 1)) i_local1 = i_group1;

	/*
	printf("i_local0 = %i\n", i_local0);
	printf("i_local1 = %i\n", i_local1);
*/
	/* copy data for work group */
	//__local struct Body local_bodies[NUM_BODIES];

	//event_t e = async_work_group_copy((__local char *)local_bodies, (char *)bodies, NUM_BODIES * sizeof(struct Body), 0);
	//wait_group_events(1, &e);
	
	/* compute */
	for(int p = i_local0; p < i_local1; p++)
	{
		struct BranchPair* pp = branches->_M_branch_pairs + p;

		struct Branch* b0 = branches->_M_branches + pp->b0;
		struct Branch* b1 = branches->_M_branches + pp->b1;

		if(!(b0->_M_flag & Branch::FLAG_IS_LEAF))
		{
			continue;
		}
		if(!(b1->_M_flag & Branch::FLAG_IS_LEAF))
		{
			continue;
		}

		if(b0->_M_mass <= 0)
		{
			pp->_M_alive = 0;
			continue;
		}
		if(b1->_M_mass <= 0)
		{
			pp->_M_alive = 0;
			continue;
		}
		
		//__local float * x0 = b0->x;
		//__local float * x1 = b1->x;
		float * x0 = b0->x;
		float * x1 = b1->x;
	
		float r[3];
		
		r[0] = x0[0] - x1[0];
		r[1] = x0[1] - x1[1];
		r[2] = x0[2] - x1[2];
	
		float d2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];


		//float dr = rsqrt(d2);
		float d = sqrt(d2);

		pp->d = sqrt(d2);
	

		if(pp->d < (b0->radius + b1->radius))
		{
			//printf("collision\n");
			pp->_M_collision = 1;
			// atomic
			b0->num_collisions++;
			// atomic
			b1->num_collisions++;
		}
		
		
		
		if(d2 == 0.0)
		{
			//printf("d2 is zero. d = %f\n", pp->d);
		}
		else
		{
			float f = 6.67384E-11 * b0->mass * b1->mass / d2;

			b0->f[0] -= f * r[0] / d;
			b0->f[1] -= f * r[1] / d;
			b0->f[2] -= f * r[2] / d;

			b1->f[0] += f * r[0] / d;
			b1->f[1] += f * r[1] / d;
			b1->f[2] += f * r[2] / d;
		}
	}

}

