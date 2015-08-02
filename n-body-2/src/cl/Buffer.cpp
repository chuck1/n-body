#include <cassert>
#include <CL/cl.h>

#include "other.hpp"

#include "cl.hpp"

Buffer::Buffer(
		std::shared_ptr<Context> c,
		std::string name,
		cl_mem m):
	_M_context(c),
	_M_name(name),
	_M_id(m)
{}
Buffer::~Buffer()
{
	release();
}
void			Buffer::enqueueWrite(
		std::shared_ptr<CommandQueue> cq,
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
			std::shared_ptr<CommandQueue> cq,
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
	printf("clReleaseMemObject\n");
	int ret = clReleaseMemObject(_M_id);
	check(__LINE__, ret);
}

