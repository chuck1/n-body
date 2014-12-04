#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "body.h"

#define FILEMODE_WRITE 0
#define FILEMODE_READ 0

float radius(float m);
void	print(float * x);

struct Frame
{
	public:
		Frame();
		Frame(Frame const & f);
		Frame &			operator=(Frame const & f);
		Body*			b(unsigned int i);
		unsigned int		size() const;
		void			alloc(int n);
		int			try_insert(
				float * x,
				float a0,
				float a1,
				float a2,
				float (*f0)(float),
				float (*f1)(float),
				float (*f2)(float),
				float radius,
				unsigned int idx);
		void			copy(Body* b, int n);
		unsigned int		reduce();
		void			random(float m, float w, float v);
		void			spin(float m, float w);
		void			rings(float m, float w);
		unsigned int	count_dead()
		{
			unsigned int n = 0;
			for(unsigned int i = 0; i < bodies_.size(); i++)
			{
				if(bodies_[i].alive == 0) n++;
			}
			return n;
		}
		unsigned int	count_alive()
		{
			unsigned int n = 0;
			for(unsigned int i = 0; i < bodies_.size(); i++)
			{
				if(bodies_[i].alive == 1) n++;
			}
			return n;
		}
		void		write(FILE* pf)
		{
			unsigned int n = bodies_.size();
			fwrite(&n, sizeof(unsigned int), 1, pf);
			fwrite(&bodies_[0], sizeof(Body), n, pf);

		}
		void		read(FILE* pf)
		{
			unsigned int n;
			fread(&n, sizeof(unsigned int), 1, pf);
			bodies_.resize(n);
			fread(&bodies_[0], sizeof(Body), n, pf);

			//print();
		}
		void		print();
		glm::vec3	body_max();
		glm::vec3	body_min();
		/*
		 * need kernel function for this operation
		 */
		int		mass_center(float * x, float * s, float * m);
	public:	
		std::vector<Body>	bodies_;
};
struct Frames
{
	Frames() {}
	Frames(Frames const & f): frames_(f.frames_)
	{
	}
	Frames &	operator=(Frames const & f)
	{
		frames_ = f.frames_;
		return *this;
	}
	void		write(FILE* pf)
	{
		unsigned int n = frames_.size();
		fwrite(&n, sizeof(unsigned int), 1, pf);
		for(unsigned int i = 0; i < n; i++)
		{
			frames_[i].write(pf);
		}
	}
	void		read(FILE* pf)
	{
		unsigned int n;
		fread(&n, sizeof(unsigned int), 1, pf);
		frames_.resize(n);
		for(unsigned int i = 0; i < n; i++)
		{
			frames_[i].read(pf);
		}
	}

	std::vector<Frame>	frames_;
};
struct Pairs
{
	void			init(Frame const & f)
	{
		assert(f.size() > 0);

		int k = 0;

		unsigned int nb = f.size();



		map_.alloc(nb);

		pairs_.resize(nb * (nb - 1) / 2);

		for(unsigned int i = 0; i < nb; i++)
		{
			for(unsigned int j = i + 1; j < nb; j++)
			{

				pairs_[k].b0 = i;
				pairs_[k].b1 = j;

				map_.pair_[i * nb + j] = k;
				map_.pair_[j * nb + i] = k;

				k++;
			}
		}
	}
	unsigned int		size() const
	{
		return pairs_.size();
	}
	std::vector<Pair>	pairs_;
	Map			map_;
};
struct Universe
{
	public:
		Universe();
		Body*			b(int t);
		Body*			b(int t, int i);
		int			solve();
		void			alloc(int num_bodies, int num_steps);
		int			mass_center(int t, float * x, float * s, float * m);
		void			write();
		int			read(std::string fileName = std::string("data.dat"), int num_steps = 0);
		void			rw_header();
		void			operator&(int i);
		void			stats();
		unsigned int		count_alive(int t);
		unsigned int		count_dead(int t);
		void			add_frame(unsigned int n);
		Frame &			get_frame(int t);
		unsigned int		size(unsigned int t);
		std::string		getFilename();


	public:
		Frames			frames_;

		static const unsigned int	NAME_SIZE = 32;

		int			num_steps_;

		int			first_step_;

		char			name_[NAME_SIZE];

		// extra timeseries data
		std::vector<glm::vec3>		mass_center_;

		// read/write
		FILE*		pfile_;
		int		filemode_;
};


#endif
