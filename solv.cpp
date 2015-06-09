#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <CL/cl.h>
#include <ctime>
#include <cstring>
#include <fstream>
#include <signal.h>

#include "universe.h"
#include "other.hpp"
#include "Branches.hpp"

#include "cl.hpp"

float timestep = 1.0;
float mass = 1e6;
float width = 100.0;

// 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768

cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue command_queue = NULL;

cl_mem memobj_bodies = NULL;
cl_mem memobj_pairs = NULL;
cl_mem memobj_map = NULL;
cl_mem memobj_flag_multi_coll = NULL;

cl_program program = NULL;

cl_kernel kernel_pairs = NULL;
cl_kernel kernel_bodies = NULL;
cl_kernel kernel_collisions = NULL;
cl_kernel kernel_clear_bodies_num_collisions = NULL;

cl_platform_id platform_id = NULL;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;

struct Problem
{
	unsigned int	_M_num_bodies;
	unsigned int	_M_num_step;
};

int		info_problem(Problem const & p)
{
	printf("problem:\n");
	printf("%39s = %16i\n", "num bodies", p._M_num_bodies);
	printf("%39s = %16i\n", "bodies per group", p._M_num_bodies / NUM_GROUPS);
	printf("%39s = %16i\n", "bodies per work item", p._M_num_bodies / NUM_GROUPS / LOCAL_SIZE);
	printf("%39s = %16lu\n", "sizeof(Universe)", sizeof(Universe));
	printf("%39s = %16lu\n", "sizeof(Branches)", sizeof(Branches));
	printf("%39s = %16lu\n", "sizeof(Branch)", sizeof(Branch));
	printf("%39s = %16lu\n", "size of Branches::_M_branches", sizeof(Branch) * BTREE_MAX_BRANCHES);
	printf("%39s = %16lu\n", "sizeof(Body)", sizeof(Body));
	//printf("%39s = %16lu\n", "sizeof(Pair)", sizeof(Pair));
	//printf("%39s = %16lu\n", "size of pairs", ((num_bodies * (num_bodies-1)) / 2 * sizeof(Pair)));
	printf("%39s = %16lu\n", "sizeof(Map)", sizeof(Map));
	return 0;
}
int		cleanup()
{
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

int should_exit = 0;

void		signal_callback(int signum)
{
	should_exit = 1;
}



int		main(int ac, char ** av)
{
	puts("Create Universe");

	Problem prob;
	
	prob._M_num_bodies = 8;
	prob._M_num_step = 10000;
	
	char ** a = av + 1;
	if(ac >= 3)
	{
		while(a < av + ac)
		{
			if(strcmp(a[0], "-b") == 0)
			{
				prob._M_num_bodies = atoi(a[1]);
			}
			else if(strcmp(a[0], "-s") == 0)
			{
				prob._M_num_step = atoi(a[1]);
			}
			else
			{
				printf("unknown option %s\n", a[0]);
				exit(1);
			}

			a += 2;
		}
	}

	//signal(SIGINT, signal_callback);

	info_problem(prob);

	Universe* uni = new Universe;

	if(ac == 2)
	{
		uni->read(av[1], prob._M_num_step);
	}
	else
	{
		time_t rawtime;
		tm * timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(uni->name_, 32, "%Y_%m_%d_%H_%M_%S", timeinfo);

		printf("name = %s\n", uni->name_);

		uni->alloc(prob._M_num_bodies, prob._M_num_step);

		//uni->random(mass);
		//uni->get_frame(0).spin(mass, width);
		uni->get_frame(0).sphere(mass, width, 0);
		//uni->get_frame(0).rings(mass, width);
	}

	/* Get Platform and Device Info */
	puts("Get Platform and Device Info");
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	check(__LINE__, ret);

	printf("clGetPlatformIDs %i\n", ret);

	if (ret) exit(1);

	//std::vector<std::string>	filenames;

	if(ret)
	{
		// files.dat filename
		char filename[128];
		strcpy(filename, "files_");
		strcat(filename, uni->name_);
		strcat(filename, ".dat");

		// CPU
		for(int i = 0; i < 100; i++)
		{
			uni->solve();

			uni->write();

			// append filename
			//filenames.push_back(u->getFilename());
			std::ofstream ofs;
			ofs.open(filename, std::ofstream::out | std::ofstream::app);
			ofs << uni->getFilename() << std::endl;
			ofs.close();

			// copy last to first
			//memcpy(u->b(0), u->b(u->num_steps_ - 1), u->num_bodies_ * sizeof(Body));

			uni->get_frame(0) = uni->get_frame(uni->num_steps_ - 1);

			// reset
			uni->frames_.frames_.resize(1);

			uni->first_step_ += uni->num_steps_;


			// check abort signal	
			if(should_exit == 1) break;
		}

		exit(0);
	}

	//printf("x = %f %f %f\n", u->bodies[0].x[0], u->bodies[0].x[1], u->bodies[0].x[2]);


	/* Get Platform and Device Info */

	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices); check(__LINE__, ret);

	/* Get Device Info */
	ret = get_device_info(device_id);
	
	Device* device = new Device(device_id);

	Context* context = device->createContext();
	
	CommandQueue* command_queue = context->createCommandQueue(device);

	/* Create Memory Buffer */
	puts("create buffer");

	Buffer* memobj_bodies = context->createBuffer(uni->size(0) * sizeof(Body));

	//memobj_pairs    = clCreateBuffer(context, CL_MEM_READ_WRITE, pairs.size() * sizeof(Pair), NULL, &ret);
	//memobj_map      = clCreateBuffer(context, CL_MEM_READ_WRITE, uni->size(0) * uni->size(0) * sizeof(unsigned int), NULL, &ret);
	
	Buffer* memobj_flag_multi_coll = context->createBuffer(sizeof(unsigned int));

	check(__LINE__, ret);

	unsigned int flag_multi_coll = 0;


	/* Write to buffers */
	puts("write buffers");
	memobj_bodies->enqueueWrite(command_queue, 0, uni->size(0) * sizeof(Body), uni->b(0));
	//ret = clEnqueueWriteBuffer(command_queue, memobj_pairs,    CL_TRUE, 0, pairs.size() * sizeof(Pair),	 &pairs.pairs_[0], 0, NULL, NULL); check(__LINE__, ret);
	//ret = clEnqueueWriteBuffer(command_queue, memobj_map,      CL_TRUE, 0, sizeof(Map),	                 &pairs.map_, 0, NULL, NULL); check(__LINE__, ret);
	memobj_flag_multi_coll->enqueueWrite(command_queue, 0, sizeof(unsigned int), &flag_multi_coll);
	
	//ret = clEnqueueWriteBuffer(command_queue, memobj_dt,       CL_TRUE, 0, sizeof(float),                    &timestep, 0, NULL, NULL); check(__LINE__, ret);
	check(__LINE__, ret);

	/* Create Kernel Program from the source */
	program = create_program_from_file(context->_M_id, device_id);

	/* Create OpenCL Kernel */
	kernel_pairs = clCreateKernel(program, "step_pairs", &ret); check(__LINE__, ret);
	kernel_bodies = clCreateKernel(program, "step_bodies", &ret); check(__LINE__, ret);
	kernel_collisions = clCreateKernel(program, "step_collisions", &ret); check(__LINE__, ret);
	kernel_clear_bodies_num_collisions = clCreateKernel(program, "clear_bodies_num_collisions", &ret); check(__LINE__, ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel_pairs, 0, sizeof(cl_mem), (void *)&memobj_bodies);
	ret = clSetKernelArg(kernel_pairs, 1, sizeof(cl_mem), (void *)&memobj_pairs);
	check(__LINE__, ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel_collisions, 0, sizeof(cl_mem), (void *)&memobj_bodies);
	ret = clSetKernelArg(kernel_collisions, 1, sizeof(cl_mem), (void *)&memobj_pairs);
	ret = clSetKernelArg(kernel_collisions, 2, sizeof(cl_mem), (void *)&memobj_flag_multi_coll);
	check(__LINE__, ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel_bodies, 0, sizeof(cl_mem), (void *)&memobj_bodies);
	ret = clSetKernelArg(kernel_bodies, 1, sizeof(cl_mem), (void *)&memobj_pairs);
	ret = clSetKernelArg(kernel_bodies, 2, sizeof(cl_mem), (void *)&memobj_map);
	ret = clSetKernelArg(kernel_bodies, 3, sizeof(float), (void *)&timestep); check(__LINE__, ret);

	ret = clSetKernelArg(kernel_clear_bodies_num_collisions, 0, sizeof(cl_mem), (void *)&memobj_bodies);

	/* Execute OpenCL Kernel */


	get_kernel_info(kernel_pairs, device_id);

	puts("execute");

	size_t global_size = GLOBAL_SIZE;
	size_t local_size = LOCAL_SIZE;

	auto program_time_start = std::chrono::system_clock::now();

	for(unsigned int t = 1; t < prob._M_num_step; t++)
	{
		puts("loop");

		if((t % (prob._M_num_step / 10)) == 0) printf("t = %5i\n", t);

		/* Execute "step_pairs" kernel */

		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_pairs, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id);
		check(__LINE__, ret);

		/* Execute "step_bodies" kernel */
		ret = clEnqueueNDRangeKernel( command_queue->_M_id, kernel_bodies, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id);
		check(__LINE__, ret);

		/* Execute "step_collisions" kernel */
		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_collisions, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);

		/* Read flag_multi_coll */
		ret = clEnqueueReadBuffer(command_queue->_M_id, memobj_flag_multi_coll->_M_id, CL_TRUE, 0, sizeof(unsigned int), &flag_multi_coll, 0, NULL, NULL); check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);


		/* Execute "clear_bodies_num_collisions" kernel */
		ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_clear_bodies_num_collisions, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);

		if(flag_multi_coll)
		{
			puts("resolve multi_coll");

			/* Execute "step_collisions" kernel on a single thread to resolve bodies with multiple collisions */
			global_size = 1;
			local_size = 1;

			ret = clEnqueueNDRangeKernel(command_queue->_M_id, kernel_collisions, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

			check(__LINE__, ret);
			if(ret) break;

			clFinish(command_queue->_M_id); check(__LINE__, ret);

			global_size = GLOBAL_SIZE;
			local_size = LOCAL_SIZE;
		}

		/* Reset flag_multi_coll */
		flag_multi_coll = 0;

		ret = clEnqueueWriteBuffer(command_queue->_M_id, memobj_flag_multi_coll->_M_id, CL_TRUE, 0, sizeof(unsigned int), &flag_multi_coll, 0, NULL, NULL); check(__LINE__, ret);
		clFinish(command_queue->_M_id); check(__LINE__, ret);


		/* Store data for timestep */
		ret = clEnqueueReadBuffer(command_queue->_M_id, memobj_bodies->_M_id, CL_TRUE, 0, uni->size(0) * sizeof(Body), uni->b(t), 0, NULL, NULL);
		check(__LINE__, ret);
		if(ret) break;

		clFinish(command_queue->_M_id); check(__LINE__, ret);
	}

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - program_time_start);

	printf("duration = %i milliseconds\n", (int)duration.count());

	/* Finalization */
	ret = clFlush(command_queue->_M_id);check(__LINE__, ret);

	if(ret)
	{
		return 1;
	}

	ret = clFinish(command_queue->_M_id);check(__LINE__, ret);

	ret = cleanup();

	/* Display Universe Data */
	/*
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
	   */

	if(ret)
	{
		return 1;
	}

	puts("Write");
	uni->write();

	return 0;
}
