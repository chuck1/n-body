#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <thread>
#include <iostream>

#include <Body.hpp>
#include "kernel.hpp"
#include "universe.h"
#include "Branch.hpp"
#include <CollisionBuffer.hpp>
#include <config.hpp>

#define DEBUG (1)

void		print(float * a)
{
	printf("% 12f % 12f % 12f\n", a[0], a[1], a[2]);
}


Universe::Universe(): first_step_(0)
{
}
Body*		Universe::b(int t)
{
	assert(frames_.frames_.size() > (unsigned int)t);
	return frames_.frames_[t].b(0); // &_M_bodies[0] + (t * num_bodies_);
}
Body*		Universe::b(int t, int i)
{
	assert(frames_.frames_.size() > (unsigned int)t);
	return frames_.frames_[t].b(i); //&_M_bodies[0] + (t * num_bodies_ + i);
}
void		Universe::alloc(int num_bodies, int num_steps)
{
	num_steps_ = num_steps;

	/* Allocate bodies */
	add_frame(num_bodies);
}
unsigned int	count_alive(Body * b, int n)
{
	int na = 0;
	for(int i = 0; i < n; i++)
	{
		if(b[i].alive == 1) na++;
	}
	return na;
}
unsigned int	count_dead(Body * b, int n)
{
	int na = 0;
	for(int i = 0; i < n; i++)
	{
		if(b[i].alive == 0) na++;
	}
	return na;
}
unsigned int	Universe::count_alive(int t)
{
	assert(frames_.frames_.size() > (unsigned int)t);
	return frames_.frames_[t].count_alive();
}
Frame &		Universe::get_frame(int t)
{
	assert(frames_.frames_.size() > (unsigned int)t);
	return frames_.frames_[t];
}
unsigned int	Universe::count_dead(int t)
{
	assert(frames_.frames_.size() > (unsigned int)t);
	return frames_.frames_[t].count_dead();
}
void		print_header()
{
	printf("%12s%16s%16s%16s%16s%16s%16s%12s%12s%12s\n",
			"dur real",
			"sim time",
			"step",
			"step inner",
			"alive",
			"dead",
			"temp_dead",
			"eta",
			"num escaped",
			"branches");
}
void		Universe::refresh_pairs(Frame & f)
{
	//float w = 20000;
	//_M_pairs.init(f);

	assert(_M_branches);

	//_M_branches->init(f, glm::vec3(-w * 0.5), glm::vec3(w * 0.5));
	_M_branches->init(f);
}



int		Universe::solve()
{
	// temp bodies

	//Body* bodies = new Body[num_bodies_];
	//memcpy(bodies, b(0), num_bodies_ * sizeof(Body));

	_M_branches.reset(new Branches);

	Frame f = get_frame(0);
	unsigned int number_removed = f.reduce();
	printf("number_removed = %i\n", number_removed);

	refresh_pairs(f);

	unsigned int threads = 4;

	unsigned int flag_multi_coll = 0;
	float dt =  1.0;


	unsigned int alive = count_alive(0);
	unsigned int dead = size(0) - alive;

	unsigned int one_tenth = size(0) / 10;
	unsigned int temp_dead = 0;

	unsigned int nc = 0;

	_M_step = first_step_;
	_M_time_sim = 0;


	float velocity_ratio_min = 0.1;
	float velocity_ratio[3];

	_M_duration_real = 0;

	printf("num_steps = %i\n", num_steps_);

	auto program_time_start = std::chrono::system_clock::now();

	float mass_center[3];
	float mass;

	unsigned int number_escaped = 0;

	CollisionBuffer cb;

	if(0)
	{
		branches()->print();
	}

	for(int t = 1; t < num_steps_; t++)
	{
		if(0) printf("universe::bytes() = %i\n", bytes());

		// dynamic time step
		// timestep set such that fastest body moves 1/10 of radius per step
		auto spd = f.get_speed_max();
		auto rad = f.get_radius_min();
		dt = rad / (100.0 * spd);
		dt = std::min(10.f, dt);

		_M_step++;

		_M_time_sim += dt;

		if(((t % (num_steps_ / 1)) == 0) || (t == 1)) print_header();

		if((t % (num_steps_ / 10)) == 0)
		{
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - program_time_start);

			_M_duration_real = (float)duration.count() / 1000.0;

			float eta =
				(_M_duration_real == 0.0) ?
				0.0 :
				(_M_duration_real / (float)(t-1) * (float)(num_steps_ - t));

			printf("%12.1f%16f%16i%16i%16i%16i%16i%12.1f%12i%12i%12f%12f%12f\n",
					_M_duration_real,
					_M_time_sim,
					_M_step,
					t,
					alive,
					dead,
					temp_dead,
					eta,
					number_escaped,
					branches()->_M_num_branches,
					spd,
					rad,
					dt
			      );
		}

		if(temp_dead >= one_tenth)
		{
			one_tenth = alive / 10;
			temp_dead = 0;

			number_removed += f.reduce();

			refresh_pairs(f);
		}

		/*
		 * Update Branches
		 */
		if(THREADED)
		{
			launch(
					threads,
					update_branches,
					branches().get(),
					f.b(0)
					);
		}
		else
		{
			update_branches(branches().get(), f.b(0));
		}

		if(1)
		{
		if(not(branches()->count_bodies() == f.count_alive()))
		{
			printf("_M_branches.count_bodies() = %i f.count_alive() = %i\n", branches()->count_bodies(), f.count_alive());
			abort();
		}
		}

		/*
		 * refresh branch masses
		 */
		if(THREADED)
		{
			launch(
					threads,
					refresh_branch_mass,
					branches().get(),
					f.b(0)
			      );
		}
		else
		{
			branches()->refresh_mass(branches().get(), f.b(0));
		}

		/* 
		 * Execute "step_pairs" kernel
		 */
		if(THREADED)
		{
			launch(
					threads,
					step_branch_pairs,
					branches().get(),
					&cb,
					f.b(0)
			      );
		}
		else
		{
			step_branch_pairs(
					branches().get(),
					&cb,
					f.b(0)
					/*&_M_pairs.pairs_[0]*/
					/*_M_pairs.map_.ptr(),*/
					/*f.size()*/
					); 
		}

		/*
		 * Execute "step_bodies" kernel
		 */
		if(0)
		{
			f.mass_center(mass_center, 0, &mass);

			std::fill_n(velocity_ratio, 3, 0);

			number_escaped = 0;
		}

		if(THREADED)
		{
			launch(
					threads,
					step_bodies,
					f.b(0),
					dt,
					f.size(),
					velocity_ratio,
					mass_center,
					mass,
					&number_escaped
			      );
		}
		else
		{
			step_bodies(
					f.b(0),
					dt,
					f.size(),
					velocity_ratio,
					mass_center,
					mass,
					&number_escaped
				   );
		}

		if(0)
		{
			if(
					(velocity_ratio[0] > velocity_ratio_min) ||
					(velocity_ratio[1] > velocity_ratio_min) ||
					(velocity_ratio[2] > velocity_ratio_min)
			  )
			{
				printf("velocity_ratio = % 12f % 12f % 12f\n",
						velocity_ratio[0],
						velocity_ratio[1],
						velocity_ratio[2]
				      );

				dt *= 0.5;
			}
		}


		/* Execute "step_collisions" kernel */
		step_collisions(
				f.b(0),
				&cb, //&_M_pairs.pairs_[0],
				&flag_multi_coll,
				&nc
				/*_M_pairs.size()*/
			       );

		/* Execute "clear_bodies_num_collisions" kernel */
		clear_bodies_num_collisions(f.b(0), f.size());

		if(flag_multi_coll)
		{
			puts("resolve multi_coll");

			/* Execute "step_collisions" kernel on a single thread to resolve bodies with multiple collisions */
			step_collisions(
					f.b(0),
					&cb, //&_M_pairs.pairs_[0],
					&flag_multi_coll,
					&nc
					/*_M_pairs.size()*/
				       );
		}

		cb._M_size = 0;

		alive -= nc;
		dead += nc;
		temp_dead += nc;
		nc = 0;

		/* Reset flag_multi_coll */
		flag_multi_coll = 0;

		if(0)
		{
			if((number_removed + f.count_dead()) != dead)
			{
				printf("1: %i %i %i\n", number_removed, f.count_dead(), dead);
				exit(1);
			}
		}

		/* Store data for timestep */
		//add_frame(num_bodies_);
		//memcpy(b(t), f.b(0), num_bodies_ * sizeof(Body));
		frames_.frames_.push_back(f);


		/*
		 * reset body force variable
		 */
		launch(
				threads,
				reset_bodies,
				f.b(0),
				dt,
				f.size(),
				velocity_ratio,
				mass_center,
				mass,
				&number_escaped
		      );



		if(frames_.frames_.size() != (unsigned int)(t+1))
		{
			printf("error\nframes_.frames_.size() = %i\n(unsigned int)(t+1) = %i\n",
					(int)frames_.frames_.size(),
					(t+1));
		}

		assert(frames_.frames_[t].bodies_.size() == f.bodies_.size());

		if(0) {
			if(count_dead(t) != f.count_dead())
			{
				printf("2: %i %i\n", count_dead(t), f.count_dead());
				exit(1);
			}

			if((number_removed + count_dead(t)) != dead)
			{
				printf("3: %i %i\n", (number_removed + count_dead(t)), dead);
				exit(1);
			}
		}
	}

	return 0;
}
void	Universe::add_frame(unsigned int n)
{
	frames_.frames_.emplace_back();
	frames_.frames_.back().alloc(n);
}
std::string	Universe::getFilename()
{
	char buffer0[16];
	char buffer[16];
	sprintf(buffer, "%i", num_steps_ + first_step_);

	memset(buffer0,   0, 16);
	memset(buffer0, '_', 16-strlen(buffer));

	char fileName[128];
	strcpy(fileName, "data/data_");
	strcat(fileName, name_);
	strcat(fileName, "_");
	strcat(fileName, buffer0);
	strcat(fileName, buffer);
	strcat(fileName, ".dat");

	return std::string(fileName);
}
void		Universe::write()
{
	auto filename = getFilename();

	FILE * pfile = fopen(filename.c_str(), "w");
	if(pfile == 0)
	{
		perror("fopen");
		printf("%s\n",filename.c_str());
		exit(1);
	}

	fwrite(&num_steps_, sizeof(int), 1, pfile);
	fwrite(&first_step_, sizeof(int), 1, pfile);

	fwrite(&name_, 1, NAME_SIZE, pfile);

	printf("name = %s\n", name_);

	frames_.write(pfile);

	fclose(pfile);
}
int		Universe::read(std::string fileName, int num_steps)
{
	FILE * pfile = fopen(fileName.c_str(), "r");
	if(!pfile) return 1;

	int num_steps_old;

	fread(&num_steps_old, sizeof(int), 1, pfile);
	fread(&first_step_, sizeof(int), 1, pfile);

	fread(&name_, 1, NAME_SIZE, pfile);

	printf("name = %s\n", name_);

	if(num_steps > 0)
	{
		// Read in order to continue simulation
		num_steps_ = num_steps;

		first_step_ += num_steps_;

		// temporary frames
		Frames frames;
		frames.read(pfile);

		// copy last to first
		frames_.frames_.push_back(frames.frames_[num_steps_old - 1]);
	}
	else
	{
		num_steps_ = num_steps_old;

		frames_.read(pfile);

		alloc(size(0), num_steps_);
	}

	fclose(pfile);
	return 0;
}
int		Universe::mass_center(int t, float * x, float * s, float * m)
{
	get_frame(t).mass_center(x, s, m);
	return 0;
}
void		Universe::stats()
{
	float m;

	mass_center_.resize(num_steps_);

	float s[3];

	for(int t = 0; t < num_steps_; t++)
	{
		mass_center(t, &mass_center_[t].x, s, &m);
	}
}
unsigned int			Universe::size(unsigned int t)
{
	assert(t < frames_.frames_.size());
	return frames_.frames_[t].bodies_.size();
}
std::shared_ptr<Branches>	Universe::branches()
{
	assert(_M_branches);
	return _M_branches;
}
unsigned int			Universe::bytes() const
{
	unsigned int b = 0;

	b += frames_.bytes();

	//b += sizeof(Branches);

	return b;
}







