#ifndef KERNEL_HPP
#define KERNEL_HPP

#ifdef CPU
#include <iostream>
#include <thread>
#include <map>
#include <thread>
#include <mutex>
#include <vector>
#endif

#include "include/decl.hpp"


#ifdef CPU
extern unsigned int				thread_count;
extern unsigned int				thread_count_temp;
extern std::map<std::thread::id, unsigned int>	thread_map;
extern std::mutex				mutex_thread_map;

extern std::mutex				g_mutex_bodies;
extern std::mutex				g_mutex_branches;

void						register_thread();

template<typename... Args> void		launch(unsigned int n, void(*f)(Args...), Args... args)
{
	std::vector<std::thread> v;

	thread_count = n;
	thread_count_temp = 0;

	auto l = [](void(*f)(Args...), Args... args)
	{
		register_thread();
		//std::cout << "hello from thread " << get_local_id(0) << std::endl;
		f(args...);
	};

	for(unsigned int i = 0; i < thread_count; i++)
	{
		v.push_back(std::thread(l, f, args...));
	}

	for(unsigned int i = 0; i < thread_count; i++)
	{
		v[i].join();
	}

	thread_count = 0;

}

int						get_num_groups(int);
int						get_group_id(int);
int						get_local_id(int);
int						get_local_size(int);
int						get_global_size(int);
float						rsqrt(float);

#endif


void						divide(
		unsigned int n,
		unsigned int * i_local0,
		unsigned int * i_local1);

void			step_bodies(
		__global struct Body * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		float dt,
		unsigned int num_bodies,
		__global float * velocity_ratio,
		__global float * mass_center,
		float mass,
		__global unsigned int * number_escaped
		);
void			reset_bodies(
		__global struct Body * bodies,
		float dt,
		unsigned int num_bodies,
		float * velocity_ratio,
		float * mass_center,
		float mass,
		unsigned int * number_escaped
		);

void			step_pairs(
		__global struct Body * bodies,
		__global struct Pair * pairs,
		unsigned int num_pairs
	       );

void			step_collisions(
		__global struct Body* bodies, /* readonly */
		__global struct CollisionBuffer * cb,
		/*struct Pair* pairs,*/
		unsigned int * flag_multi_coll,
		unsigned int * nc
		/*unsigned int num_bodies*/
		);
void			clear_bodies_num_collisions(
		__global struct Body * bodies,
		unsigned int num_bodies
		);
void			update_branches(
		__global struct Branches * branches,
		__global struct Body * bodies
		);
void			step_branch_pairs(
		__global struct Branches * branches,
		__global struct CollisionBuffer * cb,
		__global struct Body * bodies
		);
void			refresh_branch_mass(
		__global struct Branches * branches,
		__global struct Body * bodies);

#endif

