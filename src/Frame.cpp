#include <cstring>

#include "universe.h"


Frame::Frame()
{
}
Frame::Frame(Frame const & f): bodies_(f.bodies_)
{
	//printf("%s\n", __PRETTY_FUNCTION__);
}
Frame &			Frame::operator=(Frame const & f)
{
	//printf("%s\n", __PRETTY_FUNCTION__);
	bodies_ = f.bodies_;
	return *this;
}
Body*			Frame::b(unsigned int i)
{
	assert(bodies_.size() > i);
	return &bodies_[i];
}
Body const *		Frame::b(unsigned int i) const
{
	assert(bodies_.size() > i);
	return &bodies_[i];
}
unsigned int		Frame::size() const
{
	return bodies_.size();
}
void			Frame::alloc(int n)
{
	bodies_.resize(n);
}
void			Frame::copy(Body* b, int n)
{
	bodies_.resize(n);
	memcpy(&bodies_[0], b, n * sizeof(Body));
}
void			Frame::print()
{
	for(unsigned int i = 0; i < bodies_.size(); i++)
	{

		printf("x = % 12f % 12f % 12f m = % 12e r = % 12f alive = %i\n",
				bodies_[i].x[0],
				bodies_[i].x[1],
				bodies_[i].x[2],
				bodies_[i].mass,
				bodies_[i].radius,
				bodies_[i].alive);

	}

}

unsigned int		Frame::reduce()
{
	unsigned int n = 0;

	auto it = bodies_.begin();

	while(it != bodies_.end())
	{
		if(it->alive == 1)
		{
			it++;
		}
		else
		{
			it = bodies_.erase(it);
			n++;
		}
	}
	return n;
}
void			Frame::random(float m, float w, float v)
{
	for(Body & b : bodies_)
	{
		b.x[0] = (float)(rand() % (int)w) - w * 0.5;
		b.x[1] = (float)(rand() % (int)w) - w * 0.5;
		b.x[2] = (float)(rand() % (int)w) - w * 0.5;

		::print(b.x);

		if(v > 0.0)
		{
			b.v[0] = (float)(rand() % (int)v) - v * 0.5;
			b.v[1] = (float)(rand() % (int)v) - v * 0.5;
			b.v[2] = (float)(rand() % (int)v) - v * 0.5;
		}

		b.mass = m;
		b.radius = radius(b.mass);
	}
}
void			Frame::spin(float m, float w)
{
	// give bodies xz velocity orbiting mass_center

	// universe mass
	float umass = size() * m;

	for(Body & b : bodies_)
	{
		b.x[0] = (float)(rand() % (int)w) - w * 0.5;
		b.x[1] = (float)(rand() % (int)w) - w * 0.5;
		b.x[2] = (float)(rand() % (int)w) - w * 0.5;

		float r = sqrt(b.x[0] * b.x[0] + b.x[1] * b.x[1] + b.x[2] * b.x[2]);

		float rxz = sqrt(b.x[0] * b.x[0] + b.x[2] * b.x[2]);

		float v = sqrt(6.67384E-11 * umass / r) * 0.5;

		b.v[0] = -b.x[2] / rxz * v;
		b.v[1] = 0;
		b.v[2] = b.x[0] / rxz * v;

		b.mass = m;
		b.radius = radius(b.mass);
	}
}

float	distance(float * x0, float * x1)
{
	float r[3];
	r[0] = x0[0] - x1[0];
	r[1] = x0[1] - x1[1];
	r[2] = x0[2] - x1[2];
	float d = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
	return d;
}

float	func1(float a)
{
	return (float)(rand() % (int)a) - a * 0.5;
}
float	func2(float a)
{
	return a;
}

int			Frame::try_insert(
		float * x,
		float a0,
		float a1,
		float a2,
		float (*f0)(float),
		float (*f1)(float),
		float (*f2)(float),
		float radius,
		unsigned int idx)
{
	bool fail = true;

	for(unsigned int j = 0; j < 10; j ++)
	{
		x[0] = f0(a0);
		x[1] = f1(a1);
		x[2] = f2(a2);

		fail = false;

		for(unsigned int i = 0; i < idx; i++)
		{
			Body & b = bodies_[i];

			assert(b.radius);

			if(distance(b.x, x) < (b.radius + radius))
			{
				//printf("retry insert %i\n", j);
				fail = true;
				break;
			}
		}

		if(!fail) return 0;
	}

	printf("failed clean insert\n");

	return 1;
}
void			Frame::rings(float m, float w)
{
	// give bodies xz velocity orbiting mass_center

	float rad = radius(m);

	printf("radius = %f w = %f\n", rad, w);
	printf("num bodies = %i\n", size());

	// universe mass
	float umass = size() * m;

	float x[3];

	int n = 0;


	for(unsigned int i = 0; i < bodies_.size(); i++)
		//for(Body & b : bodies_)
	{
		Body & b = bodies_[i];


		if(try_insert(x, w, 0.0, w, func1, func2, func1, rad, i)) n++;

		b.x[0] = x[0];
		b.x[1] = x[1];
		b.x[2] = x[2];

		/*
		   b.x[0] = (float)(rand() % (int)w) - w * 0.5;
		   b.x[1] = 0.0;
		   b.x[2] = (float)(rand() % (int)w) - w * 0.5;
		   */
		//::print(b.x);

		float r = sqrt(b.x[0] * b.x[0] + b.x[1] * b.x[1] + b.x[2] * b.x[2]);

		float rxz = sqrt(b.x[0] * b.x[0] + b.x[2] * b.x[2]);

		float v = sqrt(6.67384E-11 * umass / r) * 0.2;

		b.v[0] = -b.x[2] / rxz * v;
		b.v[1] = 0;
		b.v[2] = b.x[0] / rxz * v;

		b.mass = m;
		b.radius = rad;
	}

	printf("%i failed inserts\n", n);
}
glm::vec3	Frame::body_max()
{
	glm::vec3 e(FLT_MIN);

	for(Body & b : bodies_)
	{
		if(b.alive)
		{
			e.x = std::max(e.x, b.x[0]);
			e.y = std::max(e.y, b.x[1]);
			e.z = std::max(e.z, b.x[2]);
		}
	}

	return e;
}

glm::vec3	Frame::body_min()
{
	glm::vec3 e(FLT_MAX);

	for(Body & b : bodies_)
	{
		if(b.alive)
		{
			e.x = std::min(e.x, b.x[0]);
			e.y = std::min(e.y, b.x[1]);
			e.z = std::min(e.z, b.x[2]);
		}
	}

	return e;
}
int		Frame::mass_center(float * x, float * s, float * m) const
{
	float temp[3] = {0,0,0};
	
	*m = 0;
	
	for(Body const & b : bodies_)
	{
		if(b.alive)
		{
			temp[0] += b.x[0] * b.mass;
			temp[1] += b.x[1] * b.mass;
			temp[2] += b.x[2] * b.mass;

			*m += b.mass;
		}
	}

	temp[0] /= *m;
	temp[1] /= *m;
	temp[2] /= *m;

	if(x)
	{
		x[0] = temp[0];
		x[1] = temp[1];
		x[2] = temp[2];
	}

	// weighted std

	if(s)
	{
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 0;

		for(Body const & b : bodies_)
		{
			if(b.alive)
			{
				temp[0] += b.mass * pow(b.x[0] - x[0], 2);
				temp[1] += b.mass * pow(b.x[1] - x[1], 2);
				temp[2] += b.mass * pow(b.x[2] - x[2], 2);
			}
		}

		s[0] = sqrt(temp[0] / *m);
		s[1] = sqrt(temp[1] / *m);
		s[2] = sqrt(temp[2] / *m);
	}

	return 0;
}
unsigned int	Frame::count_dead()
{
	unsigned int n = 0;
	for(unsigned int i = 0; i < bodies_.size(); i++)
	{
		if(bodies_[i].alive == 0) n++;
	}
	return n;
}
unsigned int	Frame::count_alive()
{
	unsigned int n = 0;
	for(unsigned int i = 0; i < bodies_.size(); i++)
	{
		if(bodies_[i].alive == 1) n++;
	}
	return n;
}
void		Frame::write(FILE* pf)
{
	unsigned int n = bodies_.size();
	fwrite(&n, sizeof(unsigned int), 1, pf);
	fwrite(&bodies_[0], sizeof(Body), n, pf);

}
void		Frame::read(FILE* pf)
{
	unsigned int n;
	fread(&n, sizeof(unsigned int), 1, pf);
	bodies_.resize(n);
	fread(&bodies_[0], sizeof(Body), n, pf);

	//print();
}

