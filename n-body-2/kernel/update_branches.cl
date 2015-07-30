
#include "include/kernel/kCollisionBuffer.hpp"

//#include <kernel.hpp>
//#include <Branches.hpp>
//#include <Body.hpp>
//#include <free.hpp>
//#include <config.hpp>

//#include <condition_variable>

#define GRAV (6.67384E-11)

#define DEBUG (0)

void		divide(unsigned int n, unsigned int * i_local0, unsigned int * i_local1)
{
	// work group
	int local_block = n / get_num_groups(0);

	int i_group0 = get_group_id(0) * local_block;
	int i_group1 = i_group0 + local_block;

	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = n;

	// work item
	int block = (i_group1 - i_group0) / get_local_size(0);

	*i_local0 = i_group0 + get_local_id(0) * block;
	*i_local1 = *i_local0 + block;

	if(get_local_id(0) == (get_local_size(0) - 1)) *i_local1 = i_group1;
}

// move bodies around in branches according to changes in position
void			update_branches(
		__global struct kBranches * branches,
		__global struct kBody * bodies
		)
{
	//rintf("%s\n", __PRETTY_FUNCTION__);

	unsigned int i_local0;
	unsigned int i_local1;

	for(int level = (int)branches->_M_lowest_level; level >= 0; level--) // for each level, starting at lowest level
	{
		// sync workgroup threads
		// only syncs the workgroup!
		barrier(CLK_GLOBAL_MEM_FENCE);

		divide(branches->_M_num_branches, &i_local0, &i_local1);
		
		// for each branch at that level
		for(unsigned int idx = i_local0; idx < i_local1; idx++) { 

			//rintf("level = %3i branch index %3i\n", level, idx);

			__global struct kBranch * branch = branches->_M_branches + idx;

			if(branch->_M_level != (unsigned int)level) continue;

			if(branch->_M_flag & KBRANCH_FLAG_IS_LEAF)
			{
				unsigned int i = 0;
				while(i < branch->_M_num_elements) {
					//rintf("i = %i branch->_M_num_elements = %i\n", i, branch->_M_num_elements);

					unsigned int body_idx = branch->_M_elements[i];

					__global struct kBody * body = bodies + body_idx;

					if(body->alive) {
						if(
								(body->x[0] < branch->_M_x0[0]) ||
								(body->x[1] < branch->_M_x0[1]) ||
								(body->x[2] < branch->_M_x0[2]) ||
								(body->x[0] > branch->_M_x1[0]) ||
								(body->x[1] > branch->_M_x1[1]) ||
								(body->x[2] > branch->_M_x1[2])
						  ) {
							//if(DEBUG) rintf("send to parent %i\n", body_idx);

							// must be atmoic somehow...
							kbranch_send_to_parent(branch, branches, bodies, i);
						} else {
							i++;
						}
					} else {
						kbranch_erase(branch, i);
					}
				}
			} else {
				unsigned int i = 0;
				while(i < branch->_M_num_elements) {
					unsigned int body_idx = branch->_M_elements[i];

					__global struct kBody * body = bodies + body_idx;

					if(
							(body->x[0] < branch->_M_x0[0]) ||
							(body->x[1] < branch->_M_x0[1]) ||
							(body->x[2] < branch->_M_x0[2]) ||
							(body->x[0] > branch->_M_x1[0]) ||
							(body->x[1] > branch->_M_x1[1]) ||
							(body->x[2] > branch->_M_x1[2])
					  ) {
						//if(DEBUG) rintf("send to parent %i\n", body_idx);

						// atmoic
						kbranch_send_to_parent(branch, branches, bodies, i);
					} else {
						//if(DEBUG) rintf("add to children %i\n", body_idx);

						// atmoic
						kbranch_add_to_children(branch, branches, bodies, body_idx);

						kbranch_erase(branch, i);
					}
					i++;
				}
			}
		}
	}
}




