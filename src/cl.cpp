
#include <CL/cl.h>

#include "other.hpp"

#include "cl.hpp"


Context*	Device::createContext()
{
	cl_int ret;
	
	cl_context id = clCreateContext(
			NULL,
			1,
			&_M_id,
			notify_context,
			NULL,
			&ret);

	Context* c = new Context(id);

	return c;
}

CommandQueue*	Context::createCommandQueue(Device* device)
{
	cl_int ret;

	cl_command_queue id = clCreateCommandQueue(_M_id, device->_M_id, 0, &ret);
	
	CommandQueue* cq = new CommandQueue(id);

	return cq;
}
Buffer*		Context::createBuffer(unsigned long size)
{
	cl_int ret;

	cl_mem id = clCreateBuffer(
			_M_id,
			CL_MEM_READ_WRITE,
			size,
			NULL,
			&ret);

	Buffer* b = new Buffer(id);

	return b;
}
void		Buffer::enqueueWrite(CommandQueue* cq, size_t offset, size_t size, const void* ptr)
{
	cl_int ret;

	ret = clEnqueueWriteBuffer(
		cq->_M_id,
		_M_id,
		CL_TRUE,
		offset, // 0
	       	size, //uni->size(0) * sizeof(Body)
		ptr, //uni->b(0)
		0,
		NULL,
		NULL);
	
	check(__LINE__, ret);
}






