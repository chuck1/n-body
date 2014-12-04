#include <cassert>

#include "Tree.h"
#include "universe.h"

Branches::Branches(): _M_num_branches(0)
{
}
void			Branches::print()
{
	_M_branches[0].print(*this);
}
void			Branches::init(Frame & f)
{
	printf("Branches::init f.size() = %i\n", f.size());

	glm::vec3 mass_center;
	glm::vec3 std;
	float mass;
	
	f.mass_center(&mass_center.x, &std.x, &mass);
	
	_M_num_branches = 1;
	
	_M_branches[0] = Branch(mass_center - std * 10.0f, mass_center + std * 10.0f);

	printf("extents:\n");
	printf("%16f%16f%16f\n",
			mass_center.x,
			mass_center.y,
			mass_center.z);
	printf("%16f%16f%16f\n",
			std.x,
			std.y,
			std.z);

	int ret;

	for(unsigned int i = 0; i < f.size(); i++)
	{
		printf("add body index %i\n", i);

		ret = _M_branches[0].add(*this, f.b(0), i);

		if(ret)
		{
			printf("not added\n");
			//abort();
		}
	}
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
		printf("%i\n", _M_num_branches + i);
		b._M_branches[i] = _M_num_branches + i;
	}

	_M_num_branches += 8;

	return 0;
}

