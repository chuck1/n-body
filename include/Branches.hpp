#ifndef BRANCHES_HPP
#define BRANCHES_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "decl.hpp"
#include <BranchPair.hpp>
#include <Branch.hpp>
#include <Map.hpp>

/**
 *
 * @warning must be kernel-safe
 *
 * in order to be kernel-safe, branches must be stored in a continuous array
 */
struct Branches
{
	Branches();
	void			init(Frame const & f);
	void			init(Frame const & f, glm::vec3 x0, glm::vec3 x1);
	void			init_pairs();
	Branch &		get_branch(Coor const & coor);
	Branch &		get_branch(unsigned int i);
	int			alloc(Branch & branch);
	void			print();

	Branch			_M_branches[BTREE_MAX_BRANCHES];
	unsigned int		_M_num_branches;

	BranchPair		_M_branch_pairs[BTREE_MAX_BRANCH_PAIRS];
	unsigned int		_M_num_branch_pairs;

	unsigned int		_M_levels[OCTREE_MAX_LEVELS][BTREE_MAX_BRANCHES];

	Map			_M_map;
};





#endif

