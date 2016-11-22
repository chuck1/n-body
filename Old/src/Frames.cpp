#include <Frames.hpp>

Frames::Frames() {}
Frames::Frames(Frames const & f): frames_(f.frames_)
{
}
Frames &	Frames::operator=(Frames const & f)
{
	frames_ = f.frames_;
	return *this;
}
void		Frames::write(FILE* pf)
{
	unsigned int n = frames_.size();
	fwrite(&n, sizeof(unsigned int), 1, pf);
	for(unsigned int i = 0; i < n; i++)
	{
		frames_[i].write(pf);
	}
}
void		Frames::read(FILE* pf)
{
	unsigned int n;
	fread(&n, sizeof(unsigned int), 1, pf);
	frames_.resize(n);
	for(unsigned int i = 0; i < n; i++)
	{
		frames_[i].read(pf);
	}
}

