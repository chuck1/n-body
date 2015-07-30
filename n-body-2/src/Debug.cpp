#include <cstdio>

#include <Debug.hpp>

void	Debug::print()
{
	printf("%12s%12s%12s\n", "n", "G", "L");
	printf("%12u%12u%12u\n", _M_n, _M_global_size, _M_local_size);

	printf("%12s%12s%12s%12s\n", "g", "l", "i0", "i1");

	for(unsigned int g = 0; g < _M_global_size; ++g) {
		for(unsigned int l = 0; l < _M_local_size; ++l) {
			printf("%12u%12u%12u%12u\n", g, l,
					_M_i_local_0[l + g * _M_local_size],
					_M_i_local_1[l + g * _M_local_size]);
		}
	}
}

