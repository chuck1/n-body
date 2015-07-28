#include <Frames.hpp>
#include <free.hpp>

Frames::Frames() {}
Frames::Frames(Frames const & f): frames_(f.frames_)
{
}
Frames &		Frames::operator=(Frames const & f)
{
	frames_ = f.frames_;
	return *this;
}
void			Frames::write(FILE* pf)
{
	unsigned int n = frames_.size();
	fwrite(&n, sizeof(unsigned int), 1, pf);
	for(unsigned int i = 0; i < n; i++)
	{
		frames_[i].write(pf);
	}
}
void			Frames::read(FILE* pf)
{
	unsigned int n;
	fread(&n, sizeof(unsigned int), 1, pf);
	frames_.resize(n);
	for(unsigned int i = 0; i < n; i++)
	{
		frames_[i].read(pf);
	}
}
unsigned int		Frames::bytes() const
{
	unsigned int b = 0;
	for(unsigned int i = 0; i < frames_.size(); i++)
	{
		b += frames_[i].bodies_.size() * sizeof(Body);
	}
	return b;
}
Stat		Frames::stat_f3(float dt)
{
	Stat s;

	s.min_ = FLT_MAX;
	s.max_ = FLT_MIN;
	s.avg_ = 0;

	unsigned int n = 0;

	for(unsigned int i = 0; i < (frames_.size() - 1); i++)
	{
		//printf("t = %i\n", i);
		
		Frame * f0 = &frames_[i];
		Frame * f1 = &frames_[i+1];
	
		//printf("%p %p\n", f0, f1);
	
		if(f0->bodies_.size() != f1->bodies_.size()) continue;
		
		//for(unsigned int j = 0; j < f0->bodies_.size(); j++)
		for(unsigned int j = 0; j < 2; j++)
		{
			Body * b0 = &f0->bodies_[j];
			Body * b1 = &f1->bodies_[j];
			
			//printf("%p %p\n", b0, b1);

			if(b0->alive)
			{
				float f3 = glm::length(b1->f - b0->f) / b0->mass / dt;

				//print(b0->f_glm);
				//print(b1->f_glm);

				//print(b1->f_glm - b0->f_glm);

				/*
				   s.min_ = std::min(s.min_, f3);
				   s.max_ = std::max(s.max_, f3);
				   s.avg_ += f3;
				   */

				float err = 5.0/12.0 * pow(dt, 3) * f3;

				float dx = glm::length(b1->x - b0->x);

				if(dx > 0.0)
				{
					float r = err / dx;

					s.min_ = std::min(s.min_, r);
					s.max_ = std::max(s.max_, r);
					s.avg_ += r;

					n++;
				}
			}
		}
	}

	s.avg_ /= (float)n;

	return s;
}


