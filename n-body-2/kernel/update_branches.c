
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

bool			bb_out_of_bounds(
		__global float * a,
		__global float * b0,
		__global float * b1)
{
	if(a[0] < b0[0]) return true;
	if(a[1] < b0[1]) return true;
	if(a[2] < b0[2]) return true;
	if(a[0] > b1[0]) return true;
	if(a[1] > b1[1]) return true;
	if(a[2] > b1[2]) return true;
	return false;
}
void			update_branch(
		__global struct kBranch * branch,
		__global struct kBranches * branches,
		__global struct kBody * bodies)
{
	// send out-of-bounds elements to parent or kill
	unsigned int i = 0;
	// check each element in the branch
	while(i < branch->_M_num_elements) {
		unsigned int body_idx = branch->_M_elements[i];
		__global struct kBody * body = bodies + body_idx;

		// check if alive		
		if(!body->alive) {
			kbranch_erase(branch, i);
			continue;
		}

		// check out-of-bounds	
		if(bb_out_of_bounds(body->x, branch->_M_x0, branch->_M_x1) ||
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
			continue;
		}
		// keep body
		i++;
	}


	if(branch->_M_flag & KBRANCH_FLAG_IS_LEAF) {
		// if the branch is a terminal node
		if(branch->_M_num_elements >= KBRANCH_ELEMENTS_LEN) {
			// if element length exceeded, fiss
			kbranch_fiss(branch, branches);
		}
	} else {
		// if not terminal node, move all elements to children
		unsigned int i = 0;
		while(i < branch->_M_num_elements) {
			unsigned int body_idx = branch->_M_elements[i];

			__global struct kBody * body = bodies + body_idx;

			// atmoic...
			kbranch_add_to_children(branch, branches, bodies, body_idx);
		}
	}
}


// move bodies around in branches according to changes in position
__kernel void			update_branches(
		__global struct kBranches * branches,
		__global struct kBody * bodies
		)
{
	//rintf("%s\n", __PRETTY_FUNCTION__);

	unsigned int i_local0;
	unsigned int i_local1;

	int ret;
	int count;

	// loop until no changes are made to tree
	while(1) {
		count = 0;
		// for each level, starting at lowest level
		for(int level = (int)branches->_M_lowest_level; level >= 0; --level)  {
			// sync workgroup threads
			// only syncs the workgroup!
			barrier(CLK_GLOBAL_MEM_FENCE);

			divide(branches->_M_num_branches, &i_local0, &i_local1);

			// for each branch at that level
			for(unsigned int idx = i_local0; idx < i_local1; idx++) { 

				__global struct kBranch * branch = branches->_M_branches + idx;

				if(branch->_M_level != (unsigned int)level) continue;

				ret = update_branch(branch, branches, bodies);

				if(ret < 0) {
					// handle error
				} else if(ret > 0) {
					++count;
				}
			}
		}

		if(count == 0) break;
	}
}




