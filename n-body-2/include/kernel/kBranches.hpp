#ifndef KBRANCHES_HPP
#define KBRANCHES_HPP

#include "include/config.hpp"
#include "include/kernel/kBranch.hpp"


/**
 *
 * @warning must be kernel-safe
 *
 * in order to be kernel-safe, branches must be stored in a continuous array
 */
struct kBranches
{
	struct kBranch		_M_branches[BTREE_MAX_BRANCHES];
	
	unsigned int		_M_num_branches;

	unsigned int		_M_lowest_level;
};


int			kbranches_alloc(
		__global struct kBranches * branches,
		__global struct kBranch * b);



#endif

