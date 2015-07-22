#include <float.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstring>
#include <iostream>

#include "universe.h"
#include <free.hpp>
#include "Branches.hpp"

static std::vector<Universe*> u;
static unsigned int universe_index = 0;

static glm::vec3 body_center;
static glm::vec3 body_extent;
static glm::vec3 body_std;

static std::vector<std::string> fileNames;





// glut_example.c
// Stanford University, CS248, Fall 2000
//
// Demonstrates basic use of GLUT toolkit for CS248 video game assignment.
// More GLUT details at http://reality.sgi.com/mjk_asd/spec3/spec3.html
// Here you'll find examples of initialization, basic viewing transformations,
// mouse and keyboard callbacks, menus, some rendering primitives, lighting,
// double buffering, Z buffering, and texturing.
//
// Matt Ginzton -- magi@cs.stanford.edu

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include "texture.h"

#define VIEWING_DISTANCE_MIN  3.0
#define TEXTURE_ID_CUBE 1



Branches* branches;
static glm::vec3 tree_x0(-5000.0f);
static glm::vec3 tree_x1( 5000.0f);




int main(int argc, char** argv)
{
	if(argc == 3) {
		if(strcmp(argv[1], "saveframe")==0) {
			Universe* u = new Universe();
			u->read(argv[2], 0);
			FILE* pf2 = fopen("sample.frame", "w");
			u->frames_.frames_[0].write(pf2);
			fclose(pf2);
			return 0;
		}
	}
	
	if(argc != 2)
	{
		printf("wrong number of arguments\n");
		exit(1);
	}
	
	// get extension
	char * c = argv[1] + strlen(argv[1]);
	while(*c != '.')
	{
		c--;
	}
	printf("ext = %s\n",c);

	char line_buffer[128];

	if(strcmp(c, ".dat") == 0)
	{
		FILE* pf = fopen(argv[1], "r");

		while(fgets(line_buffer, 128, pf))
		{
			int l = strlen(line_buffer);
			line_buffer[l-1] = 0;
			fileNames.emplace_back(line_buffer);

			printf("%s\n", line_buffer);
		}
	}
	/*
	   else if(strcmp(c, ".dat") == 0)
	   {
	   fileNames.emplace_back(argv[1]);
	   }
	   */

	int ret;

	for(auto fileName : fileNames)
	{
		Universe* utemp = new Universe;

		try {
			ret = utemp->read(fileName);
		}
		catch(std::bad_alloc & e) {
			break;
		}

		if(ret) {
			printf("read failed: %s\n", fileName.c_str());
			exit(ret);
		}

		utemp->stats();

		u.push_back(utemp);
	}

	if(u.empty())
	{
		printf("universes empty\n");
		exit(1);
	}

	printf("num_step:   %i\n", u[universe_index]->num_steps_);
	printf("num_bodies: %i\n", u[universe_index]->size(0));

	auto emin = u[universe_index]->get_frame(0).body_min();
	auto emax = u[universe_index]->get_frame(0).body_max();

	float mass;

	u[universe_index]->mass_center(0, &body_center.x, &body_std.x, &mass);
	u[universe_index]->stats();

	body_extent = emax - emin;

	//g_fViewDistance = body_extent.x;


	printf("min:         %f %f %f\n", emin.x, emin.y, emin.z);
	printf("max:         %f %f %f\n", emax.x, emax.y, emax.z);
	printf("mass center: %f %f %f\n", body_extent.x, body_extent.y, body_extent.z);
	printf("std:         %f %f %f\n", body_std.x, body_std.y, body_std.z);
	
	float dt = 10.0;

	for(auto uni : u)
	{
		std::cout << uni << std::endl;
		Stat s = uni->frames_.stat_f3(dt);
		printf("min = %e max = %e avg = %e\n", s.min_, s.max_, s.avg_);
	}

	return 0;
}




