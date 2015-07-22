#ifndef TREE_HPP
#define TREE_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "decl.hpp"
#include <Map.hpp>
#include <Octree.hpp>

/**
 * coordinate in binary-tree
 *
 * @warning must be kernel-safe
 */
struct Coor
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

struct Branches;

/**
 * branch in binary-tree
 *
 * @warning must be kernel-safe
 */
struct Branch
{
	enum
	{
		FLAG_IS_LEAF    = 1 << 0,
		FLAG_HAS_PARENT = 1 << 1,
	};
	
	Branch();
	Branch(Branch&&);
	Branch(unsigned int idx, unsigned int parent_idx, unsigned int level, glm::vec3 x0, glm::vec3 x1);
	Branch &	operator=(Branch const & b);
	void			print(Branches & b, std::string pre = std::string());
	/**
	 * form child branches and divide elements between them
	 */
	void			fiss(Branches & branches, Body const * bodies);
	/**
	 * move child branch elements to my elements and destroy branches
	 */
	void			fuse();
	int			add(Branches & branches, Body const * bodies, unsigned int body_idx);
	int			add_to_children(Branches & branches, Body const * bodies, unsigned int body_idx);
	void			remove(unsigned int i);
	// move body from this to parent
	void			erase(unsigned int i);
	void			send_to_parent(Branches * branches, Body * bodies, unsigned int i);
	bool			is_valid();
	unsigned int		get_child_branch_index(
			unsigned int i,
			unsigned int j,
			unsigned int k);
	void			mass_center(Branches * branches, Body * bodies, float * x, float * m) const;
	void			refresh_mass(Branches * branches, Body * bodies);
	unsigned int		count_bodies() const;
	/***/
	unsigned int		_M_idx;
	// parent branch idx in Branches
	unsigned int		_M_parent_idx;
	unsigned int		_M_level;
	// branch indicies
	unsigned int		_M_branches[SPLIT];
	// body indicies (indicies in body array, frame.b(i))
	// tree must be rebuilt when frame is reduced!
	// elements array must be sorted every time an element is removed!
	unsigned int		_M_elements[BTREE_LEAF_SIZE];
	// number of elements
	unsigned int		_M_num_elements;
	// extents
	union
	{
		float			_M_x0[3];
		glm::vec3		_M_x0_glm;
	};
	union
	{
		float			_M_x1[3];
		glm::vec3		_M_x1_glm;
	};
	union
	{
		float			_M_mc[3];
		glm::vec3		_M_mc_glm;
	};
	float			_M_width;
	float			_M_mass;

	/**
	 * is this a leaf (end of the line)
	 */
	unsigned char		_M_flag;
};




#endif

