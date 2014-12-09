#include <math.h>
#include <cassert>
#include <cstdio>
#include <vector>

#include "Pair.hpp"
#include "Body.hpp"
#include <CollisionBuffer.hpp>
#include <kernel.hpp>
#include <config.hpp>

unsigned int				thread_count = 0;
unsigned int				thread_count_temp = 0;
std::map<std::thread::id, unsigned int>	thread_map;
std::mutex				mutex_thread_map;

std::mutex				g_mutex_bodies;
std::mutex				g_mutex_branches;

int	get_num_groups(int d)
{
	return 1;
}
int	get_group_id(int d)
{
	return 0;
}
int	get_local_id(int d)
{
	if(thread_count)
	{
		return thread_map[std::this_thread::get_id()];
	}
	else
	{
		return 0;
	}
}
int	get_global_id(int d)
{
	if(thread_count)
	{
		return thread_map[std::this_thread::get_id()];
	}
	else
	{
		return 0;
	}
}
int	get_local_size(int d)
{
	return get_global_size(d) / get_num_groups(d);
}
int	get_global_size(int d)
{
	if(thread_count)
	{
		return thread_count;
	}
	else
	{
		return 1;
	}
}
float	rsqrt(float f)
{
	return 1.0 / sqrt(f);
}
void		register_thread()
{
	// lock
	std::lock_guard<std::mutex> lock(mutex_thread_map);

	thread_map[std::this_thread::get_id()] = thread_count_temp;

	thread_count_temp++;
}

void clear_bodies_num_collisions(
		struct Body * bodies,
		unsigned int num_bodies
		)
{
	/* global index */

	int block = num_bodies / get_global_size(0);

	int b0 = get_global_id(0) * block;
	int b1 = b0 + block;

	if(get_global_id(0) == (get_global_size(0) - 1)) b1 = num_bodies;

	//for(int b = b_local0; b < b_local1; b++)
	for(int b = b0; b < b1; b++)
	{
		bodies[b].num_collisions = 0;
	}
}

