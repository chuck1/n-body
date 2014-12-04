
#define BIM 3
#define COOR_BYTE_LEN 2
#define BTREE_BRANCH_SIZE 128

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
	
	/**
	 * form child branches and divide elements between them
	 */
	void			fiss();
	/**
	 * move child branch elements to my elements and destroy branches
	 */
	void			fuse();
	void			add(unsigned int body_idx);
	void			remove(unsigned int i);

	// parent branch idx in Branches
	unsigned int		_M_parent_idx;
	// branch indicies
	unsigned int		_M_branches[2][2];
	// body indicies
	// tree must be rebuilt when frame is reduced!
	// elements array must be sorted every time element is removed!
	unsigned int		_M_elements[BTREE_BRANCH_SIZE];
	// number of elements
	unsigned int		_M_num_elements;
	// extents
	float			_M_x0[3];
	float			_M_x1[3];
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
	void			init(Frame & f);
	Branch &		get_branch(Coor const & coor);
	Branch &		get_branch(int i);
	
	std::vector<Branch>	_M_branches;
};







