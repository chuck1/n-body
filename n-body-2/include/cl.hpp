#ifndef CL_HPP
#define CL_HPP
#include <memory>
#include <CL/cl.h>

class Buffer;
class Context;
class CommandQueue;

class Device
{
	public:
		Device(cl_device_id d): _M_id(d) {}
		std::shared_ptr<Context>		createContext();
		cl_device_id		_M_id;
};

class Context: public std::enable_shared_from_this<Context>
{
	public:
		Context(cl_context);
		~Context();
		std::shared_ptr<CommandQueue>		createCommandQueue(std::shared_ptr<Device>);
		std::shared_ptr<Buffer>			createBuffer(std::string name, unsigned long);
		cl_context		_M_id;
};

class Buffer
{
	public:
		Buffer(std::shared_ptr<Context>, std::string name, cl_mem m);
		~Buffer();
		void			enqueueWrite(
				std::shared_ptr<CommandQueue> cq,
				size_t offset,
				size_t size,
				const void* ptr);
		void			enqueueRead(
				std::shared_ptr<CommandQueue> cq,
				size_t offset,
				size_t size,
				void* ptr);
		void			release();

		std::shared_ptr<Context>	_M_context;
		std::string			_M_name;
		size_t				_M_size;
		cl_mem				_M_id;
};

class Kernel
{
	public:
		Kernel(cl_kernel k): _M_kernel(k) {}

		void			SetKernelArg(
				cl_uint arg_index,
				size_t  arg_size,
				const void* arg_value);

		cl_kernel		_M_kernel;
};

class CommandQueue
{
	public:
		CommandQueue(cl_command_queue cq): _M_id(cq) {}
		cl_command_queue	_M_id;
};
class Program
{
public:
	Program(cl_program);
	std::shared_ptr<Kernel>		createKernel(const char *);
	cl_program		_M_program;
};

#endif



