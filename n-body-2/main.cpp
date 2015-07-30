
#include <cstdio>

#include <CL/cl.h>

#include <other.hpp>
#include <cl.hpp>
#include <Body.hpp>
#include <Universe.hpp>
#include <CollisionBuffer.hpp>

//cl_context context = NULL;
//cl_command_queue command_queue = NULL;

//cl_mem memobj_bodies = NULL;
//cl_mem memobj_pairs = NULL;
//cl_mem memobj_map = NULL;
//cl_mem memobj_flag_multi_coll = NULL;

Program * program = NULL;

Kernel* kernel_step_bodies = 0;
Kernel* kernel_step_branchpairs = 0;

//cl_kernel kernel_pairs = NULL;
//cl_kernel kernel_collisions = NULL;
//cl_kernel kernel_clear_bodies_num_collisions = NULL;

cl_platform_id platform_id = NULL;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;

/*
int		cleanup() {
	cl_int ret;


	ret = clReleaseKernel(kernel_pairs);check(__LINE__, ret);
	ret = clReleaseKernel(kernel_bodies);check(__LINE__, ret);
	ret = clReleaseKernel(kernel_collisions);check(__LINE__, ret);
	ret = clReleaseKernel(kernel_clear_bodies_num_collisions);check(__LINE__, ret);

	ret = clReleaseProgram(program);check(__LINE__, ret);

	ret = clReleaseMemObject(memobj_bodies);check(__LINE__, ret);
	ret = clReleaseMemObject(memobj_pairs);check(__LINE__, ret);
	ret = clReleaseMemObject(memobj_map);check(__LINE__, ret);
	ret = clReleaseMemObject(memobj_flag_multi_coll);check(__LINE__, ret);

	ret = clReleaseCommandQueue(command_queue);check(__LINE__, ret);
	ret = clReleaseContext(context);check(__LINE__, ret);

	return ret;
}
*/

size_t global_size = 2;
size_t local_size = 1;

CommandQueue* command_queue = 0;
Context* context = 0;
Device* device = 0;

Buffer* memobj_bodies = 0;
Buffer* memobj_branches = 0;
Buffer* memobj_cb = 0;
Buffer* memobj_flag_multi_coll = 0;

void	setup()
{
	int ret;

	// Get Platform and Device Info
	printf("Get Platform and Device Info\n");
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms); check(__LINE__, ret);

	// Get Platform and Device Info

	cl_device_id device_id = NULL;

	ret = clGetDeviceIDs(
			platform_id,
			CL_DEVICE_TYPE_DEFAULT,
			1,
			&device_id,
			&ret_num_devices);

	check(__LINE__, ret);

	// Get Device Info
	ret = get_device_info(device_id);

	device = new Device(device_id);

	context = device->createContext();

	command_queue = context->createCommandQueue(device);
}
void	write(
		Universe * u,
		CollisionBuffer * cb,
		unsigned int * flag_multi_coll
		)
	/*
		std::shared_ptr<Branches>	branches,
		Body *				bodies,
		size_t				num_bodies,
		unsigned int *			flag_multi_coll
	     )*/
{

	Frame & f = u->_M_key_frame;
	
	//write(u->_M_branches, f.b(0), f.size(), &flag_multi_coll);
	
	memobj_bodies->enqueueWrite(
			command_queue,
			0,
			f.size() * sizeof(Body),
			f.b(0));

	memobj_flag_multi_coll->enqueueWrite(
			command_queue,
			0,
			sizeof(unsigned int),
			flag_multi_coll);

	memobj_branches->enqueueWrite(
			command_queue,
			0,
			sizeof(kBranches),
			u->_M_branches.get());

	memobj_cb->enqueueWrite(
			command_queue,
			0,
			sizeof(kCollisionBuffer),
			cb);

}
void	run_step_branchpairs()
{
	ret = clEnqueueNDRangeKernel(
			command_queue->_M_id,
			kernel_step_branchpairs->_M_kernel,
			1,
			NULL,
			&global_size,
			&local_size,
			0,
			NULL,
			NULL); check(__LINE__, ret);

	clFinish(command_queue->_M_id); check(__LINE__, ret);
}
void	run_step_bodies()
{
	ret = clEnqueueNDRangeKernel(
			command_queue->_M_id,
			kernel_step_bodies->_M_kernel,
			1,
			NULL,
			&global_size,
			&local_size,
			0,
			NULL,
			NULL); check(__LINE__, ret);

	clFinish(command_queue->_M_id); check(__LINE__, ret);
}
void	info()
{
	printf("sizeof(kBranches) = %lu\n", sizeof(kBranches));
}
int	main(int ac, char ** av)
{
	info();
	/*
	// temporary data to use for testing
	size_t num_bodies = 3;
	Body bodies[num_bodies];
	float timestep = 1;

	bodies[0].x = glm::vec3(1,0,0);
	bodies[1].x = glm::vec3(0,1,0);
	bodies[2].x = glm::vec3(0,0,0);
	bodies[0].v = glm::vec3(1,0,0);
	bodies[1].v = glm::vec3(0,1,0);
	bodies[2].v = glm::vec3(0,0,0);

	for(int i = 0; i < num_bodies; ++i) bodies[i].print();
	*/

	Universe * u = new Universe();
	if(u->parse_args(ac, av)) {
		printf("parse args failed\n");
		return 1;
	}
	Frame& f = u->_M_key_frame;
	// the opencl computing part and the simulation management parts
	// of the code should be very seperate.
	// 1. the boss gets a simulation into a state where it needs to be computed
	// 2. the boss passes the simulation to the compute part
	// 3. the compute part computes...
	// 4. the boss steps forward
	// 5. repeat
	
	
	
	setup();

	// needed to initialize _M_branches
	u->pre_step();

	int ret;
	
	memobj_bodies		= context->createBuffer("bodies",		f.size() * sizeof(Body));
	memobj_branches		= context->createBuffer("branches",		sizeof(kBranches));
	memobj_cb		= context->createBuffer("cb",			sizeof(kCollisionBuffer));
	memobj_flag_multi_coll	= context->createBuffer("flag_multi_coll",	sizeof(unsigned int));
	
	//memobj_bodies = context->createBuffer(u->_M_key_frame->size() * sizeof(Body));
	//memobj_pairs    = clCreateBuffer(context, CL_MEM_READ_WRITE, pairs.size() * sizeof(Pair), NULL, &ret);
	//memobj_map      = clCreateBuffer(context, CL_MEM_READ_WRITE, uni->size(0) * uni->size(0) * sizeof(unsigned int), NULL, &ret);


	// Write to buffers
	puts("write buffers");
	
	unsigned int flag_multi_coll = 0;
	CollisionBuffer cb;
	
	write(u, &cb, &flag_multi_coll);

	//ret = clEnqueueWriteBuffer(command_queue, memobj_pairs,    CL_TRUE, 0, pairs.size() * sizeof(Pair),	 &pairs.pairs_[0], 0, NULL, NULL);
	//ret = clEnqueueWriteBuffer(command_queue, memobj_map,      CL_TRUE, 0, sizeof(Map),	                 &pairs.map_, 0, NULL, NULL);
	//ret = clEnqueueWriteBuffer(command_queue, memobj_dt,       CL_TRUE, 0, sizeof(float),                    &timestep, 0, NULL, NULL);

	// Create Kernel Program from the source
	program = new Program(create_program_from_file(context->_M_id, device->_M_id));

	// Create OpenCL Kernel /
	//kernel_pairs = clCreateKernel(program, "step_pairs", &ret); check(__LINE__, ret);

	kernel_step_branchpairs = program->createKernel("step_branchpairs");
	kernel_step_bodies      = program->createKernel("step_bodies");
	
	//kernel_collisions = clCreateKernel(program, "step_collisions", &ret); check(__LINE__, ret);
	//kernel_clear_bodies_num_collisions = clCreateKernel(program, "clear_bodies_num_collisions", &ret); check(__LINE__, ret);

	// Set OpenCL Kernel Parameters 
	//ret = clSetKernelArg(kernel_pairs, 0, sizeof(cl_mem), (void *)&memobj_bodies); check(__LINE__, ret);
	//ret = clSetKernelArg(kernel_pairs, 1, sizeof(cl_mem), (void *)&memobj_pairs);

	/*
	// Set OpenCL Kernel Parameters 
	ret = clSetKernelArg(kernel_collisions, 0, sizeof(cl_mem), (void *)&memobj_bodies);
	ret = clSetKernelArg(kernel_collisions, 1, sizeof(cl_mem), (void *)&memobj_pairs);
	ret = clSetKernelArg(kernel_collisions, 2, sizeof(cl_mem), (void *)&memobj_flag_multi_coll);
	check(__LINE__, ret);
	*/

	auto num_bodies = f.size();

	// Set OpenCL Kernel Parameters 
	
	kernel_step_branchpairs->SetKernelArg(0, sizeof(cl_mem), (void *)&memobj_branches->_M_id);
	kernel_step_branchpairs->SetKernelArg(1, sizeof(cl_mem), (void *)&memobj_cb->_M_id);
	kernel_step_branchpairs->SetKernelArg(2, sizeof(cl_mem), (void *)&memobj_bodies->_M_id);
	//	__global struct kBranches * branches,
	//	__global struct kCollisionBuffer * cb,
	//	__global struct kBody * bodies

	kernel_step_bodies->SetKernelArg(0, sizeof(cl_mem),       (void *)&memobj_bodies->_M_id);
	kernel_step_bodies->SetKernelArg(1, sizeof(float),        (void *)&u->_M_timestep);
	kernel_step_bodies->SetKernelArg(2, sizeof(unsigned int), (void *)&num_bodies);
	//kernel_bodies->SetKernelArg(3, sizeof(float *),  (void *)velocity_ratio);
	//kernel_bodies->SetKernelArg(4, sizeof(float *),  (void *)mass_center);
	//kernel_bodies->SetKernelArg(5, sizeof(float),  (void *)mass);
	//kernel_bodies->SetKernelArg(6, sizeof(unsigned int*),  (void *)number_escaped);
	//ret = clSetKernelArg(kernel_bodies, 1, sizeof(cl_mem), (void *)&memobj_pairs); check(__LINE__, ret);
	//ret = clSetKernelArg(kernel_bodies, 2, sizeof(cl_mem), (void *)&memobj_map); check(__LINE__, ret);

	/*
	   ret = clSetKernelArg(kernel_clear_bodies_num_collisions, 0, sizeof(cl_mem), (void *)&memobj_bodies);
	// Execute OpenCL Kernel


	get_kernel_info(kernel_pairs, device_id);

	puts("execute");

	size_t global_size = GLOBAL_SIZE;
	size_t local_size = LOCAL_SIZE;

	auto program_time_start = std::chrono::system_clock::now();
	*/
	for(unsigned int t = 0; t < 10; t++) {
		if((t % 1) == 0) printf("t = %5i\n", t);

		u->pre_step();

		/*
		// Execute "step_pairs" kernel *

		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_pairs, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id);
		check(__LINE__, ret);
		*/
		// Execute "step_bodies" kernel
		//

		run_step_branchpairs();
		
		run_step_bodies();


		/*
		// Execute "step_collisions" kernel *
		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_collisions, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);

		// Read flag_multi_coll *
		ret = clEnqueueReadBuffer(command_queue->_M_id, memobj_flag_multi_coll->_M_id, CL_TRUE, 0, sizeof(unsigned int), &flag_multi_coll, 0, NULL, NULL); check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);


		// Execute "clear_bodies_num_collisions" kernel *
		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_clear_bodies_num_collisions, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);

		if(flag_multi_coll)
		{
		puts("resolve multi_coll");

		// Execute "step_collisions" kernel on a single thread to resolve bodies with multiple collisions *
		global_size = 1;
		local_size = 1;

		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_collisions, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);

		global_size = GLOBAL_SIZE;
		local_size = LOCAL_SIZE;
		}

		// Reset flag_multi_coll *
		flag_multi_coll = 0;

		ret = clEnqueueWriteBuffer(command_queue->_M_id, memobj_flag_multi_coll->_M_id, CL_TRUE, 0, sizeof(unsigned int), &flag_multi_coll, 0, NULL, NULL); check(__LINE__, ret);
		clFinish(command_queue->_M_id); check(__LINE__, ret);

*/
		// Store data for timestep *
		memobj_bodies->enqueueRead(
				command_queue,
				0,
				num_bodies * sizeof(Body),
				f.b(0));
		
		clFinish(command_queue->_M_id); check(__LINE__, ret);
		
		// print results
		for(size_t i = 0; i < f.size(); ++i) f.b(i)->print();
	}

	/*
	   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - program_time_start);

	   printf("duration = %i milliseconds\n", (int)duration.count());

	 * Finalization *
	 ret = clFlush(command_queue->_M_id);
	 check(__LINE__, ret);

	 if(ret)
	 {
	 return 1;
	 }

	 ret = clFinish(command_queue->_M_id);check(__LINE__, ret);

	 ret = cleanup();

	 * Display Universe Data *
	 *
	 printf("u = %f %f %f\n",
	 u->pairs[0].u[0],
	 u->pairs[0].u[1],
	 u->pairs[0].u[2]);
	 printf("f = %f\n",
	 u->pairs[0].f);
	 printf("x = %f %f %f\n",
	 u->b(NUM_STEPS - 1, 0).x[0],
	 u->b(NUM_STEPS - 1, 0).x[1],
	 u->b(NUM_STEPS - 1, 0).x[2]);
	 *

	 if(ret) { return 1; }
	
	 puts("Write");
	 uni->write();
	*/
	//ret = cleanup();

	memobj_bodies->release();

	// print results
	for(int i = 0; i < num_bodies; ++i) f.b(i)->print();

	return 0;
}


