#ifndef KBRANCH_HPP
#define KBRANCH_HPP

#define BTREE_LEAF_SIZE 16

#define KBRANCH_FLAG_IS_LEAF      1 << 0
#define KBRANCH_FLAG_HAS_PARENT   1 << 1

//#include "decl.hpp"

/**
 * coordinate in binary-tree
 *
 * @warning must be kernel-safe
 */
/*
struct kCoor
{
	unsigned char	coor(int dim, int lvl)
	{
		// get byte
		unsigned char b = _M_coor[dim][lvl / 8];
		// test bit
		return (b & (1 << (lvl % 8))) ? 1 : 0;
	}

	unsigned char	_M_coor[DIM][COOR_BYTE_LEN];
};
*/
struct kBranches;
struct kBody;

/**
 * branch in binary-tree
 *
 * @warning must be kernel-safe
 */
struct kBranch
{
	unsigned int		_M_idx;
	// parent branch idx in Branches
	unsigned int		_M_parent_idx;
	unsigned int		_M_level;
	// branch indicies
	unsigned int		_M_branches[8];
	// body indicies (indicies in body array, frame.b(i))
	// tree must be rebuilt when frame is reduced!
	// elements array must be sorted every time an element is removed!
	unsigned int		_M_elements[BTREE_LEAF_SIZE];
	// number of elements
	unsigned int		_M_num_elements;
	// extents

	float			_M_x0[3];
	float			_M_x1[3];

	float			_M_mc[3];

	float			_M_width;
	float			_M_mass;

	/**
	 * is this a leaf (end of the line)
	 */
	unsigned char		_M_flag;
};

int		kbranch_add_to_children(
			__global struct kBranch * branch,
			__global struct kBranches * branches,
			__global struct kBody const * bodies,
			unsigned int body_idx);
void		kbranch_send_to_parent(
			__global struct kBranch * branch,
			__global struct kBranches * branches,
			__global struct kBody * bodies,
			unsigned int i);
void		kbranch_erase(
			__global struct kBranch * branch,
			unsigned int i);


#endif

