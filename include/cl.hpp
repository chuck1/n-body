#ifndef CL_HPP
#define CL_HPP

#include <CL/cl.h>

class Buffer;
class Context;
class CommandQueue;

class Device
{
	public:
		Device(cl_device_id d): _M_id(d) {}
		Context*		createContext();
		cl_device_id		_M_id;
};

class Context
{
	public:
		Context(cl_context c): _M_id(c) {}
		CommandQueue*		createCommandQueue(Device*);
		Buffer*			createBuffer(unsigned long);
		cl_context		_M_id;
};

class Buffer
{
	public:
		Buffer(cl_mem m): _M_id(m) {}
		void			enqueueWrite(CommandQueue* cq, size_t offset, size_t size, const void* ptr);
		cl_mem			_M_id;
};

class CommandQueue
{
	public:
		CommandQueue(cl_command_queue cq): _M_id(cq) {}
		cl_command_queue	_M_id;
};

#endif



