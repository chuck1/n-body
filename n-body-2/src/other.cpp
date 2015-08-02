#include <sstream>
#include <map>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <CL/cl.h>

/*
#include "universe.h"
*/
#include "other.hpp"


int		get_device_info(cl_device_id device_id)
{
	cl_int ret;

	cl_uint max_compute_units;
	size_t max_work_group_size;
	cl_uint max_work_item_dimensions;
	size_t max_work_item_sizes[16];
	cl_ulong global_mem_size;
	cl_ulong local_mem_size;
	cl_ulong device_max_mem_alloc_size;
	cl_uint		device_min_data_type_align_size;
	cl_uint		device_mem_base_addr_align;

	ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS,		sizeof(cl_uint), &max_compute_units, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE,		sizeof(cl_ulong), &global_mem_size, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE,		sizeof(cl_ulong), &local_mem_size, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE,		sizeof(size_t), &max_work_group_size, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,	sizeof(cl_uint), &max_work_item_dimensions, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, max_work_item_dimensions * sizeof(size_t), &max_work_item_sizes, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE,		sizeof(cl_ulong), &device_max_mem_alloc_size, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,	sizeof(cl_uint), &device_min_data_type_align_size, NULL);
	ret = clGetDeviceInfo(device_id, CL_DEVICE_MEM_BASE_ADDR_ALIGN,		sizeof(cl_uint), &device_mem_base_addr_align, NULL);

	printf("gpu:\n");
	printf("%32s = %i\n", "max_compute_units", max_compute_units);
	printf("%32s = %i\n", "global_mem_size", (int)global_mem_size);
	printf("%32s = %i\n", "CL_DEVICE_LOCAL_MEM_SIZE", (int)local_mem_size);
	printf("%32s = %i\n", "device_max_mem_alloc_size", (int)device_max_mem_alloc_size);
	printf("%32s = %i\n", "device_min_data_type_align_size", (int)device_min_data_type_align_size);
	printf("%32s = %i\n", "device_mem_base_addr_align", (int)device_mem_base_addr_align / 8);
	printf("%32s = %i\n", "max_work_group_size", (int)max_work_group_size);
	printf("%32s = %i\n", "max_work_item_dimensions", max_work_item_dimensions);

	for(unsigned int i = 0; i < max_work_item_dimensions; i++)
	{
		printf("%29s[%i] = %i\n", "max_work_item_sizes", i, (int)max_work_item_sizes[i]);
	}

	printf("setup:\n");
	//printf("%32s = %i\n", "global size", GLOBAL_SIZE);
	//printf("%32s = %i\n", "local size", LOCAL_SIZE);
	//printf("%32s = %i\n", "num groups", NUM_GROUPS);

	check(__LINE__, ret);

	return 0;
}
int		get_kernel_info(cl_kernel kernel, cl_device_id device_id)
{
	cl_int ret;

	cl_ulong kernel_local_mem_size;

	ret = clGetKernelWorkGroupInfo(kernel, device_id,  CL_KERNEL_LOCAL_MEM_SIZE, sizeof(cl_ulong), &kernel_local_mem_size, NULL);

	printf("kernel:\n");
	printf("%32s = %i\n", "kernel local mem size", (int)kernel_local_mem_size);

	return ret;
}
cl_program create_program_from_file(cl_context context, cl_device_id device_id)
{

	cl_program program = NULL;
	cl_int ret;


	FILE *fp;
	/*
	char const * fileName[] = {
		"./include/Body.hpp",
		"./kernel/kernel.cl",
		"./kernel/step_pairs.cl",
		"./kernel/step_bodies.cl"
	};
	*/
	char const * fileName[] = {
		"./include/kernel/kBranches.hpp",
		"./include/kernel/kBody.hpp",
		"./include/kernel/kCollisionBuffer.hpp",
		"./include/kernel/vec.hpp",
		"./kernel/step_bodies.c",
		"./kernel/reset_bodies.c",
		"./kernel/step_branchpairs.c",
		"./kernel/update_branches.c",
		"./kernel/other.c",
		"./kernel/vec.c"
	};

	const size_t numFiles = sizeof fileName / sizeof(const char *);

	printf("files %i\n", (int)numFiles);

	char *source_str[numFiles];
	size_t source_size[numFiles];

	/* Load the source code containing the kernel*/

	for(unsigned int i = 0; i < numFiles; i++)
	{
		fp = fopen(fileName[i], "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel: %s.\n", fileName[i]);
			exit(1);
		}
		source_str[i] = (char*)malloc(MAX_SOURCE_SIZE);
		source_size[i] = fread(source_str[i], 1, MAX_SOURCE_SIZE, fp);

		fclose(fp);
	}

	program = clCreateProgramWithSource(
			context,
			numFiles,
			(const char **)source_str,
			(const size_t *)source_size, &ret);
	check(__LINE__, ret);



	for(unsigned int i = 0; i < numFiles; i++) free(source_str[i]);

	/* Build Kernel Program */
	ret = clBuildProgram(
			program,
			1,
			&device_id,
			"-I.",
			NULL, NULL);
	//eck(__LINE__, ret);

	cl_build_status status;
	size_t logSize;
	char* programLog;

	// build failed
	if (ret != CL_SUCCESS) {

		// check build error and build status first
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_STATUS,
				sizeof(cl_build_status), &status, NULL);

		// check build log
		clGetProgramBuildInfo(program, device_id,
				CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);

		programLog = (char*) calloc (logSize+1, sizeof(char));

		clGetProgramBuildInfo(program, device_id,
				CL_PROGRAM_BUILD_LOG, logSize+1, programLog, NULL);

		printf("Build failed; error=%d, status=%d, programLog:nn%s",
				ret, status, programLog);

		free(programLog);
	}

	return program;
}
std::string	error_string(int ec)
{
	std::map<int, std::string> m;
	m[-5] = "CL_OUT_OF_RESOURCES";
	
	auto it = m.find(ec);
	if(it == m.end()) {
		std::stringstream ss;
		ss << ec;
		return ss.str();
	} else {
		return it->second;
	}
}
void check(int line, int ret)
{
	if(ret) {
		std::string es = error_string(ret);
		printf("%i: %s\n", line, es.c_str());
		abort();
	}
}
void check(const char * f, int line, int ret, std::string msg)
{
	if(ret) {
		std::string es = error_string(ret);
		printf("%s:%i: %s: %s\n", f, line,  es.c_str(), msg.c_str());
		abort();
	}
}
void notify_context(const char * errinfo, const void * private_info, size_t cb, void * user_data)
{
	printf("Notification: %s", errinfo);
}


