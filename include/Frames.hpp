#ifndef FRAMES_HPP
#define FRAMES_HPP

#include <Frame.hpp>

struct Frames
{
	Frames();
	Frames(Frames const & f);
	Frames &		operator=(Frames const & f);
	void			write(FILE* pf);
	void			read(FILE* pf);
	unsigned int		bytes() const;
	Stat			stat_f3(float);

	std::vector<Frame>	frames_;
};

#endif

