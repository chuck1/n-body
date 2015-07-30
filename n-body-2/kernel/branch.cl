
#include "include/kernel/kBranch.hpp"

void		kbranch_send_to_parent(
			__global struct kBranch * branch,
			__global struct kBranches * branches,
			__global struct kBody * bodies,
			unsigned int i)
{
	unsigned int body_idx = branch->_M_elements[i];
	
	if(branch->_M_flag & FLAG_HAS_PARENT) {
		__globa struct kBranch * parent = branches->_M_branches[branch->_M_parent_idx];

		if(parent._M_num_elements >= BTREE_LEAF_SIZE)
		{
			//abort();
			//printf("warning: parent full\n");
			return;
		}

		// add to parent
		parent->_M_elements[parent._M_num_elements] = body_idx;
		parent->_M_num_elements++;
	} else {
		// body is outside octree, kill it
		bodies[body_idx].alive = 0;
	}

	erase(i);
}
void		kbranch_erase(
			__global struct kBranch * branch,
			unsigned int i)
{
	if(branch->_M_num_elements <= i)
	{
		//printf("i = %i _M_num_elements = %i\n", i, _M_num_elements);
		//abort();
	}

	// remove from this
	branch->_M_elements[i] = branch->_M_elements[branch->_M_num_elements - 1];
	branch->_M_num_elements--;
}
int		kbranch_add_to_children(
			__global struct kBranch * branch,
			__global struct kBranches * branches,
			__global struct kBody const * bodies,
			unsigned int body_idx)
{
	int ret;
	
	for(unsigned int i = 0; i < 8; i++) {
		//assert(branch->_M_branches[i] != branch->_M_idx);

		__global struct kBranch * b = branches->_M_branches + (branch->_M_branches[i]);

		//assert(&b != this);

		ret = kbranch_add(b, branches, bodies, body_idx);

		if(ret == 0) return 0;
	}

	// something unexpected happened. like outside entire tree

	return 1;
}
int			kbranch_add(
		__global struct kBranch * branch,
		__global struct kBranches * branches,
		__global struct kBody * bodies,
		unsigned int body_idx)
{
	__global struct kBody * b = bodies + body_idx;
	
	// the bounds of a non-leaf branch should still be valid
	// so test this first
	if(!glm::all(glm::greaterThanEqual(b->x, branch->_M_x0_glm))) return 1;

	if(!glm::all(glm::lessThan(b->x, branch->_M_x1_glm))) return 1;

	if(branch->_M_flag & FLAG_IS_LEAF) {
		if(branch->_M_num_elements == BTREE_LEAF_SIZE) {
			// this leaf is full, need to fiss
			fiss(branches, bodies);

			return kbranch_add_to_children(branch, branches, bodies, body_idx);
		} else {
			branch->_M_elements[branch->_M_num_elements] = body_idx;
			branch->_M_num_elements++;
			return 0;
		}
	}
}

