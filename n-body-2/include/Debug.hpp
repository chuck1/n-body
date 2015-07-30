#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <kernel/kDebug.hpp>

class Debug: public kDebug
{
public:
	void	print()
	{
		for(unsigned int g = 0; g < GLOBAL_SIZE; ++g) {
		for(unsigned int l = 0; l < LOCAL_SIZE; ++l) {
			printf("%lu %lu %lu %lu\n", g, l, _M_i_local_0[l + g * LOCAL_SIZE])
		}
		}
	}
};

#endif
