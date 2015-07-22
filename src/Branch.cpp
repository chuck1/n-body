#include <iostream>

#include "Branch.hpp"
#include "Branches.hpp"
#include "Body.hpp"
#include <free.hpp>
#include <debug.hpp>


Branch::Branch(): _M_branches{0,0,0,0,0,0,0,0}, _M_num_elements(0), _M_flag(FLAG_IS_LEAF)
{
	if(DEBUG_BRANCH) printf("%s %p\n", __PRETTY_FUNCTION__, this);
}
Branch::Branch(Branch && b):
	_M_idx(b._M_idx),
	_M_parent_idx(b._M_parent_idx),
	_M_level(b._M_level),
	_M_num_elements(b._M_num_elements),
	_M_x0_glm(b._M_x0_glm),
	_M_x1_glm(b._M_x1_glm),
	_M_flag(b._M_flag)
{
	if(DEBUG_BRANCH) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	memcpy(_M_branches, b._M_branches, SPLIT * sizeof(unsigned int));
	memcpy(_M_elements, b._M_elements, BTREE_LEAF_SIZE * sizeof(unsigned int));
}
Branch::Branch(unsigned int idx, unsigned int parent_idx, unsigned int level, glm::vec3 x0, glm::vec3 x1):
	_M_idx(idx),
	_M_parent_idx(parent_idx),
	_M_level(level),
	_M_branches{0,0,0,0,0,0,0,0},
	_M_num_elements(0),
	_M_x0_glm(x0),
	_M_x1_glm(x1),
	_M_flag(FLAG_IS_LEAF)
{
	if(DEBUG_BRANCH) printf("%s %p\n", __PRETTY_FUNCTION__, this);
}
Branch &	Branch::operator=(Branch const & b)
{
	if(DEBUG_BRANCH) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	_M_idx = b._M_idx;
	_M_parent_idx = b._M_parent_idx;
	_M_level = b._M_level;
	_M_num_elements = b._M_num_elements;
	_M_x0_glm = b._M_x0_glm;
	_M_x1_glm = b._M_x1_glm;
	_M_flag = b._M_flag;

	memcpy(_M_branches, b._M_branches, 8 * sizeof(unsigned int));
	memcpy(_M_elements, b._M_elements, BTREE_LEAF_SIZE * sizeof(unsigned int));

	return *this;
}
void			Branch::print(Branches & b, std::string pre)
{
	if(_M_flag & FLAG_IS_LEAF)
	{
		std::cout << pre << "leaf: " << _M_num_elements << std::endl;
	}
	else
	{
		std::cout << pre << "{" << std::endl;
		for(int i = 0; i < 8; i++)
		{
			b.get_branch(_M_branches[i]).print(b, pre + "    ");
		}
		std::cout << pre << "}" << std::endl;
	}
}
/**
 * form child branches and divide elements between them
 */
void			Branch::fiss(Branches & branches, Body const * bodies)
{
	if(DEBUG_BRANCH || 0) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	_M_flag &= ~FLAG_IS_LEAF;

	assert(!(_M_flag & FLAG_IS_LEAF));

	branches.alloc(*this);

	// distribute elements among children
	for(unsigned int i = 0; i < _M_num_elements; i++)
	{
		int ret = add_to_children(branches, bodies, _M_elements[i]);

		if(ret)
		{
			printf("not addded\n");

			::print(_M_x0);
			::print(_M_x1);

			::print(bodies[_M_elements[i]].x);

			abort();
		}
	}
	_M_num_elements = 0;
}
/**
 * move child branch elements to my elements and destroy branches
 */
void			Branch::fuse()
{
}
int			Branch::add(Branches & branches, Body const * bodies, unsigned int body_idx)
{
	Body const * b = bodies + body_idx;

	if(_M_flag & FLAG_IS_LEAF)
	{
		if(DEBUG_BRANCH) printf("add to this\n");

		if(!glm::all(glm::greaterThanEqual(b->x_glm, _M_x0_glm)))
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
			fiss(branches, bodies);

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
int			Branch::add_to_children(Branches & branches, Body const * bodies, unsigned int body_idx)
{
	if(DEBUG_BRANCH) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	int ret;

	for(unsigned int i = 0; i < 8; i++)
	{
		assert(_M_branches[i] != _M_idx);

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
void			Branch::refresh_mass(Branches * branches, Body * bodies)
{
	mass_center(branches, bodies, _M_mc, &_M_mass);
}
void			Branch::mass_center(Branches * branches, Body * bodies, float * x, float * m) const
{
	if(_M_flag & FLAG_IS_LEAF)
	{
		x[0] = 0.0;
		x[1] = 0.0;
		x[2] = 0.0;
		*m = 0.0;
		for(unsigned int i = 0; i < _M_num_elements; i++)
		{
			Body & b = bodies[_M_elements[i]];

			x[0] += b.x[0] * b.mass;
			x[1] += b.x[1] * b.mass;
			x[2] += b.x[2] * b.mass;

			*m += b.mass;
		}

		if(*m > 0.0)
		{
			x[0] /= *m;
			x[1] /= *m;
			x[2] /= *m;
		}
	}
	else
	{
		x[0] = 0.0;
		x[1] = 0.0;
		x[2] = 0.0;
		*m = 0.0;

		for(unsigned int i = 0; i < 8; i++)
		{
			float x_temp[3];
			float m_temp;

			Branch & b = branches->get_branch(_M_branches[i]);

			b.mass_center(branches, bodies, x_temp, &m_temp);

			x[0] += x_temp[0];
			x[1] += x_temp[1];
			x[2] += x_temp[2];

			*m += m_temp;
		}

		if(*m > 0.0)
		{
			x[0] /= *m;
			x[1] /= *m;
			x[2] /= *m;
		}

	}
}
void			Branch::send_to_parent(Branches * branches, Body * bodies, unsigned int i)
{
	unsigned int body_idx = _M_elements[i];

	if(_M_flag & FLAG_HAS_PARENT)
	{
		Branch & parent = branches->_M_branches[_M_parent_idx];

		if(parent._M_num_elements >= BTREE_LEAF_SIZE)
		{
			//abort();
			//printf("warning: parent full\n");
			return;
		}
		
		// add to parent
		parent._M_elements[parent._M_num_elements] = body_idx;
		parent._M_num_elements++;
	}
	else // body is outside octree, kill it
	{
		bodies[body_idx].alive = 0;
	}

	erase(i);
}
void			Branch::erase(unsigned int i)
{
	if(_M_num_elements <= i)
	{
		printf("i = %i _M_num_elements = %i\n", i, _M_num_elements);
		abort();
	}

	// remove from this
	_M_elements[i] = _M_elements[_M_num_elements - 1];
	_M_num_elements--;
}
unsigned int		Branch::count_bodies() const
{
	return _M_num_elements;
}








