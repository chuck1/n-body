#include "include/kernel/kBranch.hpp"
#include "include/kernel/kBranches.hpp"

int			kbranches_alloc(
		__global struct kBranches * branches,
		__global struct kBranch * b)
{
	/* allocate 8 new branches to be children of branch b
	 */

	//if(DEBUG_BRANCHES) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	if(!b.is_valid()) {
		assert(0);
	}

	if((_M_num_branches + 8) > BTREE_MAX_BRANCHES) {
		printf("_M_num_branches    %i\n", _M_num_branches);
		printf("BTREE_MAX_BRANCHES %i\n", BTREE_MAX_BRANCHES);
		abort();
	}

	for(unsigned int i = 0; i < 8; i++)
	{
		b._M_branches[i] = _M_num_branches;
		_M_num_branches++;
	}

	glm::vec3 x0;
	glm::vec3 x1;

	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				unsigned int idx = b.get_child_branch_index(i,j,k);

				assert(idx != b._M_idx);

				Branch & c = get_branch(idx);

				if(i == 0)
				{
					x0.x = b._M_x0_glm.x;
					x1.x = (b._M_x0_glm.x + b._M_x1_glm.x) * 0.5f;
				}
				else
				{
					x0.x = (b._M_x0_glm.x + b._M_x1_glm.x) * 0.5f;
					x1.x = b._M_x1_glm.x;
				}

