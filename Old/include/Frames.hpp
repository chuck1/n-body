#ifndef FRAMES_HPP
#define FRAMES_HPP

#include <Frame.hpp>

struct Frames
{
	Frames();
	Frames(Frames const & f);
	Frames &	operator=(Frames const & f);
	void		write(FILE* pf);
	void		read(FILE* pf);

	std::vector<Frame>	frames_;
};

#endif

