
#include <kernel.h>
#include <Branches.hpp>
#include <Body.hpp>

#define GRAV (6.67384E-11)

#define DEBUG (0)


void			update_branches(
		Branches * branches,
		Body * bodies
		)
{
	//printf("%s\n", __PRETTY_FUNCTION__);

	for(int level = (int)branches->_M_lowest_level; level >= 0; level--) // for each level, starting at lowest level
	{
		for(unsigned int idx = 0; idx < branches->_M_num_branches; idx++) // for each branch at that level
		{
			//printf("level = %3i branch index %3i\n", level, idx);

			Branch & branch = branches->_M_branches[idx];

			if(branch._M_level == (unsigned int)level)
			{
				if(branch._M_flag & Branch::FLAG_IS_LEAF)
				{
					unsigned int i = 0;
					while(i < branch._M_num_elements)
					{
						unsigned int body_idx = branch._M_elements[i];

						Body & body = bodies[body_idx];

						if(body.alive)
						{
							if(
									(body.x[0] < branch._M_x0[0]) ||
									(body.x[1] < branch._M_x0[1]) ||
									(body.x[2] < branch._M_x0[2]) ||
									(body.x[0] > branch._M_x1[0]) ||
									(body.x[1] > branch._M_x1[1]) ||
									(body.x[2] > branch._M_x1[2])
							  )
							{
								if(DEBUG) printf("send to parent %i\n", body_idx);

								branch.send_to_parent(branches, bodies, i);
							}
							else
							{
								i++;
							}
						}
						else
						{
							branch.erase(branches, i);
						}
					}
				}
				else
				{
					unsigned int i = 0;
					while(i < branch._M_num_elements)
					{
						unsigned int body_idx = branch._M_elements[i];

						Body & body = bodies[body_idx];

						if(
								(body.x[0] < branch._M_x0[0]) ||
								(body.x[1] < branch._M_x0[1]) ||
								(body.x[2] < branch._M_x0[2]) ||
								(body.x[0] > branch._M_x1[0]) ||
								(body.x[1] > branch._M_x1[1]) ||
								(body.x[2] > branch._M_x1[2])
						  )
						{
							if(DEBUG) printf("send to parent %i\n", body_idx);

							branch.send_to_parent(branches, bodies, i);
						}
						else
						{
							if(DEBUG) printf("add to children %i\n", body_idx);

							branch.add_to_children(*branches, bodies, body_idx);
							branch.erase(branches, i);
						}
						i++;
					}

				}
			}
		}
	}
}

void			step_branchpairs(
		Branches * branches,
		Body * bodies
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

		if(b0->_M_num_elements == 0)
		{
			continue;
		}
		if(b1->_M_num_elements == 0)
		{
			continue;
		}

		//__local float * x0 = b0->x;
		//__local float * x1 = b1->x;
		float * x0 = b0->_M_mc;
		float * x1 = b1->_M_mc;

		float r[3];

		r[0] = x0[0] - x1[0];
		r[1] = x0[1] - x1[1];
		r[2] = x0[2] - x1[2];

		float d2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];


		//float dr = rsqrt(d2);
		float d = sqrt(d2);

		pp->d = sqrt(d2);

		// rough size of the box
		glm::vec3 W0 = b0->_M_x1_glm - b0->_M_x0_glm;
		glm::vec3 W1 = b1->_M_x1_glm - b1->_M_x0_glm;

		float w0 = glm::length(W0); 
		float w1 = glm::length(W1); 

		float ratio = 0.25;

		if(
				(d > w0) &&
				(d > w1) &&
				((w1 / (d - w0)) < ratio) &&
				((w0 / (d - w1)) < ratio)
		  )
		{
			if(DEBUG) printf("w0 = %f\nw1 = %f\nd  = %f\n", w0, w1, d);

			//use branch 1 mass for bodies in branch 0
			for(unsigned int i = 0; i < b0->_M_num_elements; i++) // for each body in branch 0
			{
				Body * pb = bodies + b0->_M_elements[i];

				glm::vec3 D = b1->_M_mc_glm - pb->x_glm;
				float len_D = glm::length(D);

				float f = GRAV * pb->mass * b1->_M_mass / len_D / len_D;

				assert(b1->_M_mass > 0);

				if(DEBUG) printf("inter-branch body branch1 f = %f\n", f);

				pb->f[0] += f * D[0] / len_D;
				pb->f[1] += f * D[1] / len_D;
				pb->f[2] += f * D[2] / len_D;
			}

			// use branch 0 mass for bodies in b1
			for(unsigned int i = 0; i < b1->_M_num_elements; i++) // for each body in branch 0
			{
				Body * pb = bodies + b1->_M_elements[i];

				glm::vec3 D = b0->_M_mc_glm - pb->x_glm;
				float len_D = glm::length(D);

				float f = GRAV * pb->mass * b0->_M_mass / len_D / len_D;
				if(DEBUG) printf("inter-branch body branch0 f = %f\n", f);

				pb->f[0] += f * D[0] / len_D;
				pb->f[1] += f * D[1] / len_D;
				pb->f[2] += f * D[2] / len_D;
			}
		}
		else
		{
			for(unsigned int i = 0; i < b0->_M_num_elements; i++) // for each body in branch 0
			{
				for(unsigned int j = 0; j < b1->_M_num_elements; j++) // for each body in branch 0
				{
					Body * pb0 = bodies + b0->_M_elements[i];
					Body * pb1 = bodies + b1->_M_elements[j];

					assert(pb0 != pb1);

					glm::vec3 D = pb0->x_glm - pb1->x_glm;

					float len_D = glm::length(D);

					float f = GRAV * pb0->mass * pb1->mass / len_D / len_D;

					if(DEBUG) printf("inter-branch body-body f = %f\n", f);

					pb0->f[0] -= f * D[0] / len_D;
					pb0->f[1] -= f * D[1] / len_D;
					pb0->f[2] -= f * D[2] / len_D;

					pb1->f[0] += f * D[0] / len_D;
					pb1->f[1] += f * D[1] / len_D;
					pb1->f[2] += f * D[2] / len_D;
				}
			}
		}
	}


	// also must calc forces between bodies in same branch!
	for(unsigned int i = 0; i < branches->_M_num_branches; i++)
	{
		Branch & branch = branches->_M_branches[i];

		for(unsigned int j = 0; j < branch._M_num_elements; j++)
		{
			for(unsigned int k = j + 1; k < branch._M_num_elements; k++)
			{
				Body * pb0 = bodies + branch._M_elements[j];
				Body * pb1 = bodies + branch._M_elements[k];

				assert(pb0 != pb1);

				glm::vec3 D = pb0->x_glm - pb1->x_glm;

				float len_D = glm::length(D);

				float f = GRAV * pb0->mass * pb1->mass / len_D / len_D;
				if(DEBUG) printf("intra-branch f = %f\n", f);

				pb0->f[0] -= f * D[0] / len_D;
				pb0->f[1] -= f * D[1] / len_D;
				pb0->f[2] -= f * D[2] / len_D;

				pb1->f[0] += f * D[0] / len_D;
				pb1->f[1] += f * D[1] / len_D;
				pb1->f[2] += f * D[2] / len_D;
			}
		}
	}
}






