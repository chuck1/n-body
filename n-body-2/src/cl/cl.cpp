#include <cassert>
#include <CL/cl.h>

#include "other.hpp"

#include "cl.hpp"

std::shared_ptr<Context>	Device::createContext()
{
	cl_int ret;
	
	cl_context id = clCreateContext(
			NULL,
			1,
			&_M_id,
			notify_context,
			NULL,
			&ret);
	check(__LINE__, ret);

	std::shared_ptr<Context> c(new Context(id));

	return c;
}

Context::Context(cl_context c): _M_id(c)
{}
Context::~Context()
{
	//printf("clReleaseContext\n");
	int ret = clReleaseContext(_M_id);
	check(__LINE__, ret);
}
std::shared_ptr<CommandQueue>	Context::createCommandQueue(std::shared_ptr<Device> device)
{
	cl_int ret;

	cl_command_queue id = clCreateCommandQueue(_M_id, device->_M_id, 0, &ret);
	check(__LINE__, ret);
	
	std::shared_ptr<CommandQueue> cq(new CommandQueue(id));

	return cq;
}
std::shared_ptr<Buffer>		Context::createBuffer(
			std::string name,
			unsigned long size)
{
	cl_int ret;

	cl_mem id = clCreateBuffer(
			_M_id,
			CL_MEM_READ_WRITE,
			size,
			NULL,
			&ret);
	
	check(__LINE__, ret);
	
	std::shared_ptr<Buffer> b(new Buffer(shared_from_this(), name, id));
	b->_M_size = size;

	return b;
}


