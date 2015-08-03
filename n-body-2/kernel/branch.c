
#include "include/kernel/kBranch.hpp"

int		kbranch_send_to_parent(
			__global struct kBranch * branch,
			__global struct kBranches * branches,
			__global struct kBody * bodies,
			unsigned int i)
{
	unsigned int body_idx = branch->_M_elements[i];
	
	if(branch->_M_flag & KBRANCH_FLAG_HAS_PARENT) {
		__global struct kBranch * parent = branches->_M_branches + branch->_M_parent_idx;

		if(parent->_M_num_elements >= BTREE_LEAF_SIZE) {
			//abort();
			//printf("warning: parent full\n");
			return EC_PARENT_FULL;
		}

		// add to parent
		parent->_M_elements[parent->_M_num_elements] = body_idx;
		parent->_M_num_elements++;
	} else {
		// body is outside octree, kill it
		bodies[body_idx].alive = 0;
	}

	kbranch_erase(branch, i);

	return 0;
}
int		kbranch_erase(
			__global struct kBranch * branch,
			unsigned int i)
{
	if(branch->_M_num_elements <= i) {
		//printf("i = %i _M_num_elements = %i\n", i, _M_num_elements);
		//abort();
		return EC_BRANCH_ELEMENTS_OUT_OF_RANGE;
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
	
	// try to put the element in each child branch [b]
	for(unsigned int i = 0; i < 8; i++) {

		if(branch->_M_branches[i] == branch->_M_idx) return -666;

		__global struct kBranch * b = branches->_M_branches + branch->_M_branches[i];

		if(b == branch) return -666;

		// try to add body to b
		ret = kbranch_add(b, branches, bodies, body_idx);

		// error check
		if(ret < 0) return ret;
		
		// body didnt belong in b
		if(ret > 0) continue;

		// successfully added to [b]; delete from [branch]
		kbranch_erase(branch, i);
		return 0;
	}

	// something unexpected happened. like outside entire tree
	return -666;
}
int			kbranch_add(
		__global struct kBranch * branch,
		__global struct kBranches * branches,
		__global struct kBody const * bodies,
		unsigned int body_idx)
{
	__global struct kBody const * b = bodies + body_idx;

	//__local float x0[3];
	//__local float x1[3];
	//async_work_group_copy(x0, branch->_M_x0, 3, 0);
	//async_work_group_copy(x1, branch->_M_x1, 3, 0);
	
	// the bounds of a non-leaf branch should still be valid
	// so test this first
	if(!vec_all_greater_than_eq_2(b->x, branch->_M_x0)) return 1;

	if(!vec_all_less_than_2(b->x, branch->_M_x1)) return 1;

	// add body to branch regardless of whether it is a terminator or not
	// because recursion is not allowed. Instead, make multiple passes in which
	// non-terminal nodes send their children down the tree
	//if(branch->_M_flag & KBRANCH_FLAG_IS_LEAF) {
		if(branch->_M_num_elements >= KBRANCH_ELEMENTS_LEN_PLUS_BUFFER) {
			// this leaf is full, need to fiss
			// NO this is recursion
			//kbranch_fiss(branch, branches, bodies);
			
			// NO this is recursion, wait for another pass
			//return kbranch_add_to_children(branch, branches, bodies, body_idx);
			return EC_BRANCH_ELEMENT_LEN_PLUS_BUFFER_EXCEEDED;
		} else {
			branch->_M_elements[branch->_M_num_elements] = body_idx;
			branch->_M_num_elements++;
			return 0;
		}
	//}
}
void			kbranch_fiss(
		__global struct kBranch * branch,
		__global struct kBranches * branches,
		__global struct kBody const * bodies)
{
	//if(DEBUG_BRANCH || 0) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	branch->_M_flag &= ~KBRANCH_FLAG_IS_LEAF;

	//assert(!(_M_flag & KBRANCH_FLAG_IS_LEAF));

	// allocate my children
	kbranches_alloc(branches, branch);

	// distribute elements among children
	// NO! that would be recursion, wait for another pass
	if(0) {
	for(unsigned int i = 0; i < branch->_M_num_elements; i++) {
		int ret = kbranch_add_to_children(
			branch,
			branches,
			bodies,
			branch->_M_elements[i]);

		if(ret) {
			/*
			printf("not addded\n");
			::print(_M_x0);
			::print(_M_x1);
			::print(bodies[_M_elements[i]].x);
			*/
			//abort();?????????????????
		}
	}
	branch->_M_num_elements = 0;
	}
}

