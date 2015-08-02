#include <cassert>
#include <CL/cl.h>

#include "other.hpp"

#include "cl.hpp"

void			Kernel::SetKernelArg(
		cl_uint arg_index,
		size_t  arg_size,
		const void* arg_value)
{
	int ret = clSetKernelArg(_M_kernel, arg_index, arg_size, arg_value);
	check(__FILE__, __LINE__, ret, "");
}

