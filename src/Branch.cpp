#include "Tree.h"
#include "body.h"

Branch::Branch(): _M_branches{0,0,0,0,0,0,0,0}, _M_num_elements(0), _M_flag(FLAG_IS_LEAF)
{
	//printf("%s %p\n", __PRETTY_FUNCTION__, this);
}
Branch::Branch(Branch && b):
	_M_num_elements(b._M_num_elements),
	_M_x0_glm(b._M_x0_glm),
	_M_x1_glm(b._M_x1_glm),
	_M_flag(b._M_flag)
{
	//printf("%s %p\n", __PRETTY_FUNCTION__, this);
	memcpy(_M_branches, b._M_branches, SPLIT * sizeof(unsigned int));
	memcpy(_M_elements, b._M_elements, BTREE_LEAF_SIZE * sizeof(unsigned int));
}
Branch &	Branch::operator=(Branch const & b)
{
	//printf("%s %p\n", __PRETTY_FUNCTION__, this);

	_M_num_elements = b._M_num_elements;
	_M_x0_glm = b._M_x0_glm;
	_M_x1_glm = b._M_x1_glm;
	_M_flag = b._M_flag;
	memcpy(_M_branches, b._M_branches, SPLIT * sizeof(unsigned int));
	memcpy(_M_elements, b._M_elements, BTREE_LEAF_SIZE * sizeof(unsigned int));

	return *this;
}
Branch::Branch(glm::vec3 x0, glm::vec3 x1): _M_branches{0,0,0,0,0,0,0,0}, _M_num_elements(0), _M_x0_glm(x0), _M_x1_glm(x1), _M_flag(FLAG_IS_LEAF)
{
	//printf("%s %p\n", __PRETTY_FUNCTION__, this);
}
void			Branches::print()
{
	if(_M_
}
/**
 * form child branches and divide elements between them
 */
void			Branch::fiss(Branches & branches)
{
	//printf("%s %p\n", __PRETTY_FUNCTION__, this);

	_M_flag &= ~FLAG_IS_LEAF;

	branches.alloc(*this);
	
	//printf("%i %i %i %i %i %i %i %i\n",
			_M_branches[0],
			_M_branches[1],
			_M_branches[2],
			_M_branches[3],
			_M_branches[4],
			_M_branches[5],
			_M_branches[6],
			_M_branches[7]);

	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				Branch & b = branches.get_branch(get_child_branch_index(i,j,k));
				
				if(i == 0)
				{
					b._M_x0_glm.x = _M_x0_glm.x;
					b._M_x1_glm.x = (_M_x0_glm.x + _M_x1_glm.x) * 0.5f;
				}
				else
				{
					b._M_x0_glm.x = (_M_x0_glm.x + _M_x1_glm.x) * 0.5f;
					b._M_x1_glm.x = _M_x1_glm.x;
				}
				if(j == 0)
				{
					b._M_x0_glm.y = _M_x0_glm.y;
					b._M_x1_glm.y = (_M_x0_glm.y + _M_x1_glm.y) * 0.5f;
				}
				else
				{
					b._M_x0_glm.y = (_M_x0_glm.y + _M_x1_glm.y) * 0.5f;
					b._M_x1_glm.y = _M_x1_glm.y;
				}
				if(k == 0)
				{
					b._M_x0_glm.z = _M_x0_glm.z;
					b._M_x1_glm.z = (_M_x0_glm.z + _M_x1_glm.z) * 0.5f;
				}
				else
				{
					b._M_x0_glm.z = (_M_x0_glm.z + _M_x1_glm.z) * 0.5f;
					b._M_x1_glm.z = _M_x1_glm.z;
				}
			}
		}
	}
}
/**
 * move child branch elements to my elements and destroy branches
 */
void			Branch::fuse()
{
}
int			Branch::add(Branches & branches, Body * bodies, unsigned int body_idx)
{

	Body * b = bodies + body_idx;

	if(_M_flag & FLAG_IS_LEAF)
	{
		//printf("add to this\n");

		if(!glm::all(glm::greaterThan(b->x_glm, _M_x0_glm)))
		{
		/*
			printf("(%12.1f%12.1f%12.1f) < (%12.1f%12.1f%12.1f)\n",
					b->x_glm.x,
					b->x_glm.y,
					b->x_glm.z,
					_M_x0_glm.x,
					_M_x0_glm.y,
					_M_x0_glm.z);
			*/		
			return 1;
		}

		if(!glm::all(glm::lessThan(b->x_glm, _M_x1_glm)))
		{
			/*
			printf("(%12.1f%12.1f%12.1f) > (%12.1f%12.1f%12.1f)\n",
					b->x_glm.x,
					b->x_glm.y,
					b->x_glm.z,
					_M_x1_glm.x,
					_M_x1_glm.y,
					_M_x1_glm.z);
			*/		
			return 1;
		}

		if(_M_num_elements == BTREE_LEAF_SIZE)
		{
			// this leaf is full, need to fiss
			fiss(branches);

			return add_to_children(branches, bodies, body_idx);
		}
		else
		{
		
			_M_elements[_M_num_elements] = body_idx;

			_M_num_elements++;

			//printf("num_elements = %i\n", _M_num_elements);

			return 0;
		}
	}
	else
	{
		return add_to_children(branches, bodies, body_idx);
	}

	return 0;
}
int			Branch::add_to_children(Branches & branches, Body * bodies, unsigned int body_idx)
{
	//printf("add_to_children\n");

	int ret;

	for(unsigned int i = 0; i < SPLIT; i++)
	{
		Branch & b = branches.get_branch(_M_branches[i]);
		
		assert(&b != this);

		ret = b.add(branches, bodies, body_idx);


		if(ret == 0) return 0;
	}

	// something unexpected happened. like outside entire tree
	//printf("wtf!\n");

	return 1;
}
unsigned int		Branch::get_child_branch_index(
		unsigned int i,
		unsigned int j,
		unsigned int k)
{
	assert(i<2);
	assert(j<2);
	assert(k<2);
	return _M_branches[i * 4 + j * 2 + k];
}

