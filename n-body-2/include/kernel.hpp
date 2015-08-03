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

#define EC_PARENT_FULL				-1;
#define EC_BRANCH_ELEMENTS_OUT_OF_RANGE		-2;

/*
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
*/


void			step_bodies(
		__global struct kBody * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		float dt,
		unsigned int num_bodies,
		__global struct kDebug * db
	//	__global float * velocity_ratio,
	//	__global float * mass_center,
	//	float mass,
	//	__global unsigned int * number_escaped
		);
void			reset_bodies(
		__global struct kBody * bodies,
//		float dt,
		unsigned int num_bodies,
		__global struct kDebug * db
//		float * velocity_ratio,
//		float * mass_center,
//		float mass,
//		unsigned int * number_escaped
		);

/*
void			step_pairs(
		__global struct kBody * bodies,
		__global struct Pair * pairs,
		unsigned int num_pairs
	       );
*/
void			step_collisions(
		__global struct kBody* bodies, /* readonly */
		__global struct kCollisionBuffer * cb,
		/*struct Pair* pairs,*/
		unsigned int * flag_multi_coll,
		unsigned int * nc
		/*unsigned int num_bodies*/
		);
void			clear_bodies_num_collisions(
		__global struct kBody * bodies,
		unsigned int num_bodies
		);
void			update_branches(
		__global struct kBranches * branches,
		__global struct kBody * bodies
		);
void			step_branchpairs(
		__global struct kBranches * branches,
		__global struct kCollisionBuffer * cb,
		__global struct kBody * bodies,
		unsigned int num_bodies
		);
void			refresh_branch_mass(
		__global struct kBranches * branches,
		__global struct kBody * bodies);

#endif

