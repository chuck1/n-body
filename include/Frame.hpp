#ifndef FRAME_HPP
#define FRAME_HPP

#include <cstdio>
#include <vector>

#include <glm/glm.hpp>

#include <Body.hpp>
#include <decl.hpp>

struct Stat
{
	float	min_;
	float	max_;
	float	avg_;
};

struct Frame
{
	public:
		Frame();
		Frame(Frame const & f);
		Frame &			operator=(Frame const & f);
		Body*			b(unsigned int i);
		Body const *		b(unsigned int i) const;
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
		void			sphere(float m, float w, float v);
		void			spin(float m, float w);
		void			rings(float m, float w);
		unsigned int		count_dead();
		unsigned int		count_alive();
		void			write(FILE* pf);
		void			read(FILE* pf);
		void			print();
		glm::vec3		body_max();
		glm::vec3		body_min();
		/*
		 * need kernel function for this operation
		 */
		int			mass_center(float * x, float * s, float * m) const;
	public:	
		std::vector<Body>	bodies_;
};

#endif
