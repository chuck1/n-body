#include <float.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstring>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <Universe.hpp>
#include <free.hpp>
#include <Branches.hpp>

static std::vector<Universe*> u;
Frame* frame;
static unsigned int universe_index = 0;

static glm::vec3 body_center;
static glm::vec3 body_extent;
static glm::vec3 body_std;

static std::vector<std::string> fileNames;

int resolution = 16;

static int ct = 0;

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include "texture.h"

#define VIEWING_DISTANCE_MIN  3.0
#define TEXTURE_ID_CUBE 1

enum {
	MENU_LIGHTING = 1,
	MENU_POLYMODE,
	MENU_TEXTURING,
	MENU_EXIT,
	MENU_RESTART
};
enum MODE {
	UNIVERSE,
	FRAME
};

typedef int BOOL;
#define TRUE 1
#define FALSE 0

static int g_t_skip = 1;
static BOOL g_bLightingEnabled = TRUE;
static BOOL g_bFillPolygons = TRUE;
static BOOL g_bTexture = FALSE;
static BOOL g_bButton1Down = FALSE;
static int g_Width = 600;                          // Initial window width
static int g_Height = 600;                         // Initial window height
static int g_xClick = 0;
static int g_yClick = 0;
static float g_lightPos[4] = { 10, 10, -100, 1 };  // Position of light
static float	g_yaw = 0.0;
static float	g_pitch = 0.0;
static float	g_radiusScale = 1.0;
#ifdef _WIN32
static DWORD last_idle_time;
#else
static struct timeval last_idle_time;
#endif

static unsigned int	g_flags = 0;
struct FLAG
{
	enum
	{
		RENDER_BRANCH = 1 << 0,
	};
};



Branches* branches;
static glm::vec3 tree_x0(-5000.0f);
static glm::vec3 tree_x1( 5000.0f);

void		render_branch(Branch & b);

inline void	RenderObjects2(int t)
{
	Frame & f = u[universe_index]->get_frame(t);


	//render_frame(f);

	if(g_flags & FLAG::RENDER_BRANCH)
	{	
		//if(t == 0)
		{
			branches->init(f, tree_x0, tree_x1);
		}


		for(unsigned int i = 0; i < branches->_M_num_branches; i++)
		{
			Branch & b = branches->get_branch(i);
		}
	}
}
void		render_branch(Branch & b)
{
	if(b._M_flag & Branch::FLAG_IS_LEAF) {
		if(b._M_num_elements > 0) {
			glm::vec3 c = (b._M_x0_glm + b._M_x1_glm) * 0.5f;
			glm::vec3 w = b._M_x1_glm - b._M_x0_glm;

			if(0) {
				print(c);
				print(w);
			}

		}
	}
}
inline void display()
{

	//glm::vec3 c = body_center;

	Universe * un = u[universe_index];
	assert(u.size() > universe_index);
	assert(un);
	
	glm::vec3 c = un->mass_center_[ct];


	{


		// Render the scene
		RenderObjects2(ct);
		ct += g_t_skip;
		while(ct >= u[universe_index]->frames_.frames_.size())
		{


			ct -= u[universe_index]->frames_.frames_.size();
			universe_index++;
			if(universe_index == u.size()) universe_index = 0;

			printf("universe_index = %i\n", universe_index);

			assert(u[universe_index]);
		}

		float axes_length = 1000.0;

	}

}


inline void AnimateScene(void)
{
	float dt;

#ifdef _WIN32
	DWORD time_now;
	time_now = GetTickCount();
	dt = (float) (time_now - last_idle_time) / 1000.0;
#else
	// Figure out time elapsed since last call to idle function
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
		1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
#endif

	// Animate the teapot by updating its angles

	// Save time_now for next time
	last_idle_time = time_now;

}

inline void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:             // ESCAPE key
			exit (0);
			break;

		case 'l':
			break;

		case 'p':
			break;

		case 't':
			break;
		case 'r':
			break;
		case ',':
			g_t_skip--;
			g_t_skip = (g_t_skip < 1) ? 1 : g_t_skip;
			break;
		case '.':
			g_t_skip++;
			break;
		case '1':
			g_radiusScale = 1.0;
			break;
		case '2':
			g_radiusScale = 2.0;
			break;
		case '3':
			g_radiusScale = 3.0;
			break;
	}
}
void	read_universes()
{
}
std::string	split_ext(std::string s)
{
	size_t p = s.find('.');
	if(p == std::string::npos) {
		assert(0);
	}
	return s.substr(p);
}
int main(int argc, char** argv)
{
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("res", po::value<int>(), "sphere resolution")
		("input-file", po::value< std::vector< std::string > >(), "input file")
		;
	
	po::positional_options_description p;
	p.add("input-file", 1);
	
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm); 
	
	if(vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}
	if(vm.count("res")) {
		resolution = vm["res"].as<int>();
	}
	
	auto input = vm["input-file"].as< std::vector < std::string > >();

	auto e = split_ext(input[0]);
	
	std::cout << "extension = " << e << "\n";

	//if(argc != 2)
	//{
	//	printf("wrong number of arguments\n");
	//	exit(1);
	//}

	// get extension
	char * c = argv[1] + strlen(argv[1]);
	while(*c != '.')
	{
		c--;
	}
	printf("ext = %s\n",c);

	char line_buffer[128];

	if(strcmp(e.c_str(), ".dat") == 0)
	{
		//FILE* pf = fopen(argv[1], "r");
		FILE* pf = fopen(input[0].c_str(), "r");
		while(fgets(line_buffer, 128, pf))
		{
			int l = strlen(line_buffer);
			line_buffer[l-1] = 0;
			fileNames.emplace_back(line_buffer);
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
		} catch(std::bad_alloc & e) {
			break;
		}

		if(ret) {
			printf("read failed: %s\n", fileName.c_str());
			exit(ret);
		}

		utemp->stats();

		u.push_back(utemp);
	}

	if(u.empty()) {
		printf("universes empty\n");
		exit(1);
	}

	printf("num_step:   %lu\n", u[universe_index]->frames_.frames_.size());
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
	printf("mass center: %f %f %f\n", body_center.x, body_center.y, body_center.z);
	printf("std:         %f %f %f\n", body_std.x, body_std.y, body_std.z);


	//u[universe_index]->list(u.num_step-100);

	// GLUT Window Initialization:
	//glutInit (&argc, argv);

	// Get the initial time, for use by animation
#ifdef _WIN32
	last_idle_time = GetTickCount();
#else
	gettimeofday(&last_idle_time, NULL);
#endif

	// Turn the flow of control over to GLUT

	return 0;
}




