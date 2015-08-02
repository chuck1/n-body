#include <cassert>
#include <CL/cl.h>

#include "other.hpp"

#include "cl.hpp"


Program::Program(cl_program p):
	_M_program(p)
{}
std::shared_ptr<Kernel>		Program::createKernel(
		const char * name)
{
	int ret;
	cl_kernel temp = clCreateKernel(_M_program, name, &ret);
	check(__FILE__, __LINE__, ret, name);
	return std::shared_ptr<Kernel>(new Kernel(temp));
}


