#include <glm/glm.hpp>
#include <vector>

#include "decl.hpp"

#define DIM 3
#define SPLIT 8
#define COOR_BYTE_LEN 2
#define BTREE_LEAF_SIZE 64
#define BTREE_MAX_BRANCHES 64


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
		FLAG_IS_LEAF = 1 << 0,
	};
	
	Branch();
	Branch(Branch&&);
	Branch(glm::vec3 x0, glm::vec3 x1);
	Branch &	operator=(Branch const & b);
	void			print();
	/**
	 * form child branches and divide elements between them
	 */
	void			fiss(Branches & branches);
	/**
	 * move child branch elements to my elements and destroy branches
	 */
	void			fuse();
	int			add(Branches & branches, Body * bodies, unsigned int body_idx);
	int			add_to_children(Branches & branches, Body * bodies, unsigned int body_idx);
	void			remove(unsigned int i);
	unsigned int		get_child_branch_index(
			unsigned int i,
			unsigned int j,
			unsigned int k);
	// parent branch idx in Branches
	unsigned int		_M_parent_idx;
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

	/**
	 * is this a leaf (end of the line)
	 */
	unsigned char		_M_flag;
};

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
	Branch &		get_branch(Coor const & coor);
	Branch &		get_branch(unsigned int i);
	int			alloc(Branch & branch);
	void			print();

	Branch			_M_branches[BTREE_MAX_BRANCHES];
	unsigned int		_M_num_branches;
};







