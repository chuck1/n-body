#ifndef MAP_HPP
#define MAP_HPP

#include <cstdio>
#include <vector>

struct Map
{
	Map();
	Map(Map&& m);
	Map &			operator=(Map const & m);
	void			alloc(unsigned int n);
	unsigned int *		ptr();

	std::vector<unsigned int>		pair_;
};


#endif


