#include <cassert>

#include "Branch.hpp"
#include "universe.h"
#include <free.hpp>
#include <debug.hpp>

Branches::Branches(): _M_num_branches(0), _M_lowest_level(0)
{
	//memset(_M_num_at_level, 0, OCTREE_LEVELS * sizeof(unsigned int));
}
void			Branches::print()
{
	_M_branches[0].print(*this);
}
void			Branches::init(Frame & f)
{
	//printf("Branches::init f.size() = %i\n", f.size());

	glm::vec3 mass_center;
	glm::vec3 std;
	float mass;
	
	f.mass_center(&mass_center.x, &std.x, &mass);
	
	glm::vec3 x0 = mass_center - std * 10.0f;
	glm::vec3 x1 = mass_center + std * 10.0f;

	/*
	printf("extents:\n");
	printf("%16f%16f%16f\n",
			mass_center.x,
			mass_center.y,
			mass_center.z);
	printf("%16f%16f%16f\n",
			std.x,
			std.y,
			std.z);
	*/

	init(f, x0, x1);
}
/*
void			Branches::level_insert(unsigned int idx, unsigned int level)
{
	assert(_M_num_at_level[level] < BTREE_MAX_BRANCHES);
	
	_M_levels[level][_M_num_at_level[level]] = idx;
	
	_M_num_at_level[level]++;
}
*/
void			Branches::init(Frame & f, glm::vec3 x0, glm::vec3 x1)
{
	_M_num_branches = 1;
	
	_M_branches[0] = Branch(0, 0, 0, x0, x1);

	int ret;

	for(unsigned int i = 0; i < f.size(); i++)
	{
		if(DEBUG_BRANCHES) printf("add body index %i\n", i);

		ret = _M_branches[0].add(*this, f.b(0), i);

		if(ret)
		{
			printf("not added\n");

			if(0)
			{
				print();

				::print(_M_branches[0]._M_x0);
				::print(_M_branches[0]._M_x1);

				::print(f.b(i)->x);

				abort();
			}

			f.b(i)->alive = 0;
		}
	}

	init_pairs();
}
Branch &		Branches::get_branch(unsigned int i)
{
	if(i >= _M_num_branches)
	{
		printf("_M_branches.size() <= i   %i %i\n", (int)_M_num_branches, (int)i);
		abort();
	}
	return _M_branches[i];
}
int			Branches::alloc(Branch & b)
{
	if(DEBUG_BRANCHES) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	assert((_M_num_branches + 8) <= BTREE_MAX_BRANCHES);

	for(unsigned int i = 0; i < 8; i++)
	{
		b._M_branches[i] = _M_num_branches;
		_M_num_branches++;
	}

	glm::vec3 x0;
	glm::vec3 x1;

	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				unsigned int idx = b.get_child_branch_index(i,j,k);

				assert(idx != b._M_idx);

				Branch & c = get_branch(idx);

				if(i == 0)
				{
					x0.x = b._M_x0_glm.x;
					x1.x = (b._M_x0_glm.x + b._M_x1_glm.x) * 0.5f;
				}
				else
				{
					x0.x = (b._M_x0_glm.x + b._M_x1_glm.x) * 0.5f;
					x1.x = b._M_x1_glm.x;
				}
				if(j == 0)
				{
					x0.y = b._M_x0_glm.y;
					x1.y = (b._M_x0_glm.y + b._M_x1_glm.y) * 0.5f;
				}
				else
				{
					x0.y = (b._M_x0_glm.y + b._M_x1_glm.y) * 0.5f;
					x1.y = b._M_x1_glm.y;
				}
				if(k == 0)
				{
					x0.z = b._M_x0_glm.z;
					x1.z = (b._M_x0_glm.z + b._M_x1_glm.z) * 0.5f;
				}
				else
				{
					x0.z = (b._M_x0_glm.z + b._M_x1_glm.z) * 0.5f;
					x1.z = b._M_x1_glm.z;
				}

				if(DEBUG_BRANCHES)
				{
					::print(x0);
					::print(x1);
				}

				c = Branch(idx, b._M_idx, b._M_level + 1, x0, x1);
				c._M_flag |= Branch::FLAG_HAS_PARENT;


				if((b._M_level + 1) > _M_lowest_level) _M_lowest_level = b._M_level + 1;
			}
		}
	}

	return 0;
}
void			Branches::init_pairs()
{
	if(DEBUG_BRANCHES) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	int k = 0;

	unsigned int nb = _M_num_branches;

	_M_map.alloc(nb);

	_M_num_branch_pairs = nb * (nb - 1) / 2;

	for(unsigned int i = 0; i < nb; i++)
	{
		for(unsigned int j = i + 1; j < nb; j++)
		{
			_M_branch_pairs[k].b0 = i;
			_M_branch_pairs[k].b1 = j;

			_M_map.pair_[i * nb + j] = k;
			_M_map.pair_[j * nb + i] = k;

			k++;
		}
	}
}
unsigned int		Branches::count_bodies() const
{
	unsigned int c = 0;

	for(unsigned int i = 0; i < _M_num_branches; i++)
	{
		c += _M_branches[i].count_bodies();
	}

	return c;
}
void			Branches::refresh_mass(Branches * branches, Body * bodies)
{
	for(unsigned int i = 0; i < _M_num_branches; i++)
	{
		_M_branches[i].refresh_mass(branches, bodies);
	}
}



