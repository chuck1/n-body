#ifndef KDEBUG_H
#define KDEBUG_H

#include "include/config.hpp"

struct kDebug
{
	// store the local indicies for every worker
	unsigned int	_M_i_local_0[GLOBAL_SIZE * LOCAL_SIZE];
	unsigned int	_M_i_local_1[GLOBAL_SIZE * LOCAL_SIZE];
};

#endif
