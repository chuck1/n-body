
#include <kernel.hpp>
#include <Branches.hpp>

/*
 * starting at lowest level ensures that child masses are refreshed before their parents
 */
void			refresh_branch_mass(
		Branches * branches,
		Body * bodies)
{
	unsigned int i_local0;
	unsigned int i_local1;

	for(int level = (int)branches->_M_lowest_level; level >= 0; level--) // for each level, starting at lowest level
	{
		divide(branches->_M_num_branches, &i_local0, &i_local1);
		
		for(unsigned int idx = i_local0; idx < i_local1; idx++) // for each branch at that level
		{
			//printf("level = %3i branch index %3i\n", level, idx);

			Branch & branch = branches->_M_branches[idx];

			if(branch._M_level == (unsigned int)level)
			{
				branch.refresh_mass(branches, bodies);
			}
		}
	}
}


