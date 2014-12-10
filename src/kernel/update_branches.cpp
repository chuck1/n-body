
#include <CollisionBuffer.hpp>
#include <kernel.hpp>
#include <Branches.hpp>
#include <Body.hpp>
#include <free.hpp>
#include <config.hpp>
#include <condition_variable>

#define GRAV (6.67384E-11)

#define DEBUG (0)

std::mutex			g_sync;
std::condition_variable		g_sync_cv;
//std::atomic_int			g_atom_count = 0;
unsigned int				g_count = 0;
void		sync()
{
	std::unique_lock<std::mutex> lock(g_sync);
	g_count++;
	if(g_count == thread_count)
	{
		//std::cout << "notifying" << std::endl;
		g_sync_cv.notify_all();
		g_count = 0;
	}
	else
	{
		//std::cout << "waiting..." << std::endl;
		g_sync_cv.wait(lock);
	}
}

void		divide(unsigned int n, unsigned int & i_local0, unsigned int & i_local1)
{
	/* work group */
	int local_block = n / get_num_groups(0);

	int i_group0 = get_group_id(0) * local_block;
	int i_group1 = i_group0 + local_block;

	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = n;

	/* work item */
	int block = (i_group1 - i_group0) / get_local_size(0);

	i_local0 = i_group0 + get_local_id(0) * block;
	i_local1 = i_local0 + block;

	if(get_local_id(0) == (get_local_size(0) - 1)) i_local1 = i_group1;
}

void			update_branches(
		Branches * branches,
		Body * bodies
		)
{
	//printf("%s\n", __PRETTY_FUNCTION__);

	unsigned int i_local0;
	unsigned int i_local1;

	for(int level = (int)branches->_M_lowest_level; level >= 0; level--) // for each level, starting at lowest level
	{
		// sync threads here
		sync();

		divide(branches->_M_num_branches, i_local0, i_local1);

		for(unsigned int idx = i_local0; idx < i_local1; idx++) // for each branch at that level
		{
			//printf("level = %3i branch index %3i\n", level, idx);

			Branch & branch = branches->_M_branches[idx];

			if(branch._M_level != (unsigned int)level) continue;

			if(branch._M_flag & Branch::FLAG_IS_LEAF)
			{
				unsigned int i = 0;
				while(i < branch._M_num_elements)
				{
					//printf("i = %i branch._M_num_elements = %i\n", i, branch._M_num_elements);

					unsigned int body_idx = branch._M_elements[i];

					Body & body = bodies[body_idx];

					if(body.alive)
					{
						if(
								(body.x[0] < branch._M_x0[0]) ||
								(body.x[1] < branch._M_x0[1]) ||
								(body.x[2] < branch._M_x0[2]) ||
								(body.x[0] > branch._M_x1[0]) ||
								(body.x[1] > branch._M_x1[1]) ||
								(body.x[2] > branch._M_x1[2])
						  )
						{
							if(DEBUG) printf("send to parent %i\n", body_idx);

							// atmoic
#if(THREADED)
							std::lock_guard<std::mutex> lock(g_mutex_branches);
#endif
							branch.send_to_parent(branches, bodies, i);
						}
						else
						{
							i++;
						}
					}
					else
					{
						branch.erase(i);
					}
				}
			}
			else
			{
				unsigned int i = 0;
				while(i < branch._M_num_elements)
				{
					unsigned int body_idx = branch._M_elements[i];

					Body & body = bodies[body_idx];

					if(
							(body.x[0] < branch._M_x0[0]) ||
							(body.x[1] < branch._M_x0[1]) ||
							(body.x[2] < branch._M_x0[2]) ||
							(body.x[0] > branch._M_x1[0]) ||
							(body.x[1] > branch._M_x1[1]) ||
							(body.x[2] > branch._M_x1[2])
					  )
					{
						if(DEBUG) printf("send to parent %i\n", body_idx);

						// atmoic
#if(THREADED)
						std::lock_guard<std::mutex> lock(g_mutex_branches);
#endif
						branch.send_to_parent(branches, bodies, i);
					}
					else
					{
						if(DEBUG) printf("add to children %i\n", body_idx);

						{
							// atmoic
#if(THREADED)
							std::lock_guard<std::mutex> lock(g_mutex_branches);
#endif
							branch.add_to_children(*branches, bodies, body_idx);
						}

						branch.erase(i);
					}
					i++;
				}
			}
		}
	}
}





