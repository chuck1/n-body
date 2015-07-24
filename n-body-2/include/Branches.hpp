#ifndef BRANCHES_HPP
#define BRANCHES_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "decl.hpp"
#include <Branch.hpp>
//#include <Map.hpp>
#include <config.hpp>

/**
 *
 * @warning must be kernel-safe
 *
 * in order to be kernel-safe, branches must be stored in a continuous array
 */
struct Branches
{
	Branches();
	void			init(Frame & f);
	void			init(Frame & f, glm::vec3 x0, glm::vec3 x1);
	//Branch &		get_branch(Coor const & coor);
	Branch &		get_branch(unsigned int i);
	int			alloc(Branch & branch);
	void			print();
	void			update();
	void			level_insert(unsigned int idx, unsigned int level);
	unsigned int		count_bodies() const;
	void			refresh_mass(Branches * branches, Body * bodies);

	Branch			_M_branches[BTREE_MAX_BRANCHES];
	unsigned int		_M_num_branches;

	//BranchPair		_M_branch_pairs[BTREE_MAX_BRANCH_PAIRS];
	//unsigned int		_M_num_branch_pairs;

	/*
	unsigned int		_M_levels[OCTREE_LEVELS][BTREE_MAX_BRANCHES];
	unsigned int		_M_num_at_level[OCTREE_LEVELS];
	*/

	// use function instead
	//Map			_M_map;
	
	unsigned int		_M_lowest_level;
};





#endif

