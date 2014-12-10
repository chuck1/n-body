#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Body.hpp"
#include <Map.hpp>
#include <Branch.hpp>
#include <Branches.hpp>
#include <Frames.hpp>


float	radius(float m);
//void	print(float * x);




struct Universe
{
	public:
		Universe();
		Body*				b(int t);
		Body*				b(int t, int i);
		int				solve();
		void				alloc(int num_bodies, int num_steps);
		int				mass_center(int t, float * x, float * s, float * m);
		void				write();
		int				read(std::string fileName = std::string("data.dat"), int num_steps = 0);
		void				stats();
		unsigned int			count_alive(int t);
		unsigned int			count_dead(int t);
		void				add_frame(unsigned int n);
		Frame &				get_frame(int t);
		unsigned int			size(unsigned int t);
		std::string			getFilename();
		void				refresh_pairs(Frame & f);
		std::shared_ptr<Branches>	branches();
		unsigned int			bytes() const;
	public:
		Frames				frames_;

		static const unsigned int	NAME_SIZE = 32;

		int				num_steps_;

		int				first_step_;

		char				name_[NAME_SIZE];

		// extra timeseries data
		std::vector<glm::vec3>		mass_center_;
		
		//Pairs				_M_pairs;
		std::shared_ptr<Branches>	_M_branches;


		// temp simulation data
		unsigned int			_M_step;
		float				_M_duration_real;
		float				_M_time_sim;
};


#endif
