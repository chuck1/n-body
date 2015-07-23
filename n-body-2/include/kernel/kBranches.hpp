#ifndef K_BRANCHES_HPP
#define K_BRANCHES_HPP

//#include "decl.hpp"
#include "include/kernel/kBranch.hpp"

#define BTREE_MAX_BRANCHES 32000

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





#endif

