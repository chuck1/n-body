#include <cassert>

#include "Tree.h"
#include "universe.h"

Branches::Branches():_M_num_branches(0)
{
}
void			Branches::print()
{
	_M_branches[0].print(*this);
}
void			Branches::init(Frame const & f)
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
void			Branches::init(Frame const & f, glm::vec3 x0, glm::vec3 x1)
{
	_M_num_branches = 1;
	
	_M_branches[0] = Branch(x0, x1);
	_M_branches[0]._M_idx = 0;
	
	int ret;

	for(unsigned int i = 0; i < f.size(); i++)
	{
		//printf("add body index %i\n", i);

		ret = _M_branches[0].add(*this, f.b(0), i);

		if(ret)
		{
			//printf("not added\n");
			//abort();
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
	assert((_M_num_branches + 8) <= BTREE_MAX_BRANCHES);
	
	for(unsigned int i = 0; i < 8; i++)
	{
		unsigned int idx = _M_num_branches + i;
		b._M_branches[i] = idx;
		_M_branches[idx]._M_parent_idx = b._M_idx;
	}

	_M_num_branches += 8;

	return 0;
}
void			Branches::init_pairs()
{

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






