#ifndef KDEBUG_H
#define KDEBUG_H

#include "include/config.hpp"

struct kDebug
{
	// store the local indicies for every worker
	unsigned int	_M_global_size;
	unsigned int	_M_local_size;
	unsigned int	_M_n;
	unsigned int	_M_i_local_0[MAX_GLOBAL_SIZE * MAX_LOCAL_SIZE];
	unsigned int	_M_i_local_1[MAX_GLOBAL_SIZE * MAX_LOCAL_SIZE];
};

#endif
