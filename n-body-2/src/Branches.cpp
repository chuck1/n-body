#include <cassert>

#include <Branch.hpp>
#include <Universe.hpp>
#include <free.hpp>
//#include <debug.hpp>

Branches::Branches():
	_M_num_branches(0),
	_M_lowest_level(0)
{
	//if(DEBUG_BRANCHES || 1) printf("%s %p\n", __PRETTY_FUNCTION__, this);
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

	if(std.x == 0) {
		assert(0);
	}
	if(std.y == 0) {
		assert(0);
	}
	if(std.z == 0) {
		assert(0);
	}

	glm::vec3 x0 = mass_center - std * 10.0f;
	glm::vec3 x1 = mass_center + std * 10.0f;

	if(0) {
	printf("extents:\n");
	printf("%16f%16f%16f\n",
			mass_center.x,
			mass_center.y,
			mass_center.z);
	printf("%16f%16f%16f\n",
			std.x,
			std.y,
			std.z);
	}

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

	auto bb = f.get_bounding_box();
	
	auto w = glm::length(bb.b - bb.a);
	
	auto W = glm::vec3(w);
	
	//_M_branches[0] = Branch(0, 0, 0, x0, x1);
	// using bounding box ensures that all bodies are contained
	_M_branches[0] = Branch(0, 0, 0, bb.a - W, bb.b + W);

	//printf("%s\n", __PRETTY_FUNCTION__);
	//printf("  % 12.2e% 12.2e% 12.2e\n",W.x, W.y, W.z);

	if(!_M_branches[0].is_valid()) {
		assert(0);
	}

	int ret;

	for(unsigned int i = 0; i < f.size(); i++) {
		//if(DEBUG_BRANCHES) printf("add body index %i\n", i);

		ret = _M_branches[0].add(*this, f.b(0), i);

		if(ret) {
			// if count not be added to branches, destroy the body
			
			//printf("not added\n");

			if(0) {
				print();

				::print(_M_branches[0]._M_x0);
				::print(_M_branches[0]._M_x1);

				::print(f.b(i)->x);

				abort();
			}

			f.b(i)->alive = 0;
		}
	}
}
Branch &		Branches::get_branch(unsigned int i)
{
	if(i >= _M_num_branches)
	{
		printf("_M_branches.size() <= i   %i %i\n", (int)_M_num_branches, (int)i);
		assert(0);
	}
	return _M_branches[i];
}
int			Branches::alloc(Branch & b)
{
	/* allocate 8 new branches to be children of branch b
	 */


	//if(DEBUG_BRANCHES) printf("%s %p\n", __PRETTY_FUNCTION__, this);

	if(!b.is_valid()) {
		assert(0);
	}

	if((_M_num_branches + 8) > BTREE_MAX_BRANCHES) {
		printf("_M_num_branches    %i\n", _M_num_branches);
		printf("BTREE_MAX_BRANCHES %i\n", BTREE_MAX_BRANCHES);
		assert(0);
	}

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

				/*if(DEBUG_BRANCHES) {
					printf("  %i %i %i\n", i,j,k);
					::print(x0);
					::print(x1);
				}*/

				c = Branch(idx, b._M_idx, b._M_level + 1, x0, x1);
				c._M_flag |= Branch::FLAG_HAS_PARENT;
				c._M_width = glm::length(c._M_x1_glm - c._M_x0_glm);


				if((b._M_level + 1) > _M_lowest_level) _M_lowest_level = b._M_level + 1;
			}
		}
	}

	return 0;
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


