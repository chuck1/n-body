#include <stdio.h>
#include <stdlib.h>
#include <chrono>
//#include <CL/cl.h>
#include <ctime>
#include <cstring>
#include <fstream>
#include <signal.h>
#include <iostream>

#include <boost/program_options.hpp>

#include "universe.h"
#include "other.hpp"
#include "Branches.hpp"

namespace po = boost::program_options;

//#include "cl.hpp"

float timestep = 1.0;
float mass = 1e6;
float width = 5000.0;

// 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768


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

int should_exit = 0;

void		signal_callback(int signum)
{
	should_exit = 1;
}

int		main_cpu(Universe* uni)
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

	return 0;
}
int		main(int ac, char ** av)
{
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("setup", po::value<std::string>(), "set compression level")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm); 

	if(vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}


	puts("Create Universe");

	Problem prob;

	prob._M_num_bodies = 10*10*10;
	prob._M_num_step = 1000;

	/*
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
	*/
	//signal(SIGINT, signal_callback);

	info_problem(prob);

	// create universe and perform some initialization (some of which should be removed)
	Universe* uni = new Universe;
	time_t rawtime;
	tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(uni->name_, 32, "%Y_%m_%d_%H_%M_%S", timeinfo);

	printf("name = %s\n", uni->name_);

	uni->alloc(prob._M_num_bodies, prob._M_num_step);


	// determine how to setup problem based on commandline args
	if (vm.count("setup")) {
		auto setup = vm["setup"].as<std::string>();
		std::cout << "setup = " << setup << "\n";

		std::map< std::string, std::function<void()> > m;

		m["hcp"] = [&] () {

		};

	} else {

		if(ac == 2)
		{
			uni->read(av[1], prob._M_num_step);
		}
		else
		{

			//uni->random(mass);
			//uni->get_frame(0).spin(mass, width);
			uni->get_frame(0).hexagonal_close_packed(mass, glm::vec3(0,0,0), glm::vec3());
			//uni->get_frame(0).sphere(mass, width, 0);
			//uni->get_frame(0).collision_coarse(mass, glm::vec3(width, 20, 0), -.02);
			//uni->get_frame(0).rings(mass, width);

			if(0) {
				FILE* pf = fopen("sample.frame", "r");
				uni->get_frame(0).read(pf);
				fclose(pf);
			}
		}

	}

	main_cpu(uni);


}

