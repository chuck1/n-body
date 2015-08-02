#include <cassert>
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
	check(__LINE__, ret);

	Context* c = new Context(id);

	return c;
}

CommandQueue*	Context::createCommandQueue(Device* device)
{
	cl_int ret;

	cl_command_queue id = clCreateCommandQueue(_M_id, device->_M_id, 0, &ret);
	check(__LINE__, ret);
	
	CommandQueue* cq = new CommandQueue(id);

	return cq;
}
Buffer*		Context::createBuffer(
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
	
	Buffer* b = new Buffer(name, id);
	b->_M_size = size;

	return b;
}
void			Buffer::enqueueWrite(
		CommandQueue* cq,
		size_t offset,
		size_t size,
		const void * ptr)
{
	assert(size == _M_size);

	assert(ptr != NULL);

	cl_int ret = clEnqueueWriteBuffer(
		cq->_M_id,
		_M_id,
		CL_TRUE,
		offset, // 0
	       	size, //uni->size(0) * sizeof(Body)
		ptr, //uni->b(0)
		0,
		NULL,
		NULL);
	
	check(__FILE__, __LINE__, ret, _M_name);
}
void		Buffer::enqueueRead(
			CommandQueue* cq,
			size_t offset,
			size_t size,
			void* ptr)
{
	assert(size == _M_size);

	assert(ptr != NULL);

	cl_int ret = clEnqueueReadBuffer(
			cq->_M_id,
			_M_id,
			CL_TRUE,
			offset, //0,
			size, //uni->size(0) * sizeof(Body),
			ptr, //uni->b(t),
			0,
			NULL,
			NULL);
	
	check(__FILE__, __LINE__, ret, _M_name);

	clFinish(cq->_M_id);

	check(__FILE__, __LINE__, ret, _M_name);
}

void		Buffer::release()
{
	int ret = clReleaseMemObject(_M_id);
	check(__LINE__, ret);
}

void			Kernel::SetKernelArg(
		cl_uint arg_index,
		size_t  arg_size,
		const void* arg_value)
{
	int ret = clSetKernelArg(_M_kernel, arg_index, arg_size, arg_value);
	check(__FILE__, __LINE__, ret, "");
}
Program::Program(cl_program p):
	_M_program(p)
{}
Kernel *		Program::createKernel(
		const char * name)
{
	int ret;
	cl_kernel temp = clCreateKernel(_M_program, name, &ret);
	check(__FILE__, __LINE__, ret, name);
	return new Kernel(temp);
}


