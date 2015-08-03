
#include "include/kernel/kCollisionBuffer.hpp"
#include "include/kernel/kBranch.hpp"
#include "include/kernel/other.hpp"

//#include <kernel.hpp>
//#include <Branches.hpp>
//#include <Body.hpp>
//#include <free.hpp>
//#include <config.hpp>

//#include <condition_variable>

#define GRAV (6.67384E-11)

#define DEBUG (0)


// move bodies around in branches according to changes in position
__kernel void			update_branches(
		__global struct kBranches * branches,
		__global struct kBody * bodies
		)
{
	//rintf("%s\n", __PRETTY_FUNCTION__);

	unsigned int i_local0;
	unsigned int i_local1;
	
	// for each level, starting at lowest level
	for(int level = (int)branches->_M_lowest_level; level >= 0; level--)  {
		// sync workgroup threads
		// only syncs the workgroup!
		barrier(CLK_GLOBAL_MEM_FENCE);

		divide(branches->_M_num_branches, &i_local0, &i_local1);
		
		// for each branch at that level
		for(unsigned int idx = i_local0; idx < i_local1; idx++) { 
			
			__global struct kBranch * branch = branches->_M_branches + idx;

			if(branch->_M_level != (unsigned int)level) continue;

			if(branch->_M_flag & KBRANCH_FLAG_IS_LEAF) {
				unsigned int i = 0;
				while(i < branch->_M_num_elements) {
					// if the branch is a terminal node, check each element in the branch

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
							// body is outside the boundaries; send it to parent
							// must be atmoic somehow...
							//   maybe the parent should have a seperate array of elements
							//   for each child branch to store these bodies
							kbranch_send_to_parent(branch, branches, bodies, i);
						} else {
							// keep body
							i++;
						}
					} else {
						// body is dead, delete it
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
						// body is outside the boundaries; send it to parent
						// atmoic...
						kbranch_send_to_parent(branch, branches, bodies, i);
					} else {
						//if(DEBUG) rintf("add to children %i\n", body_idx);

						// atmoic...
						kbranch_add_to_children(branch, branches, bodies, body_idx);

						kbranch_erase(branch, i);
					}
				}
			}
		}
	}
}




