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

#include <GL/glut.h>
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
static GLfloat g_fTeapotAngle = 0.0;
static GLfloat g_fTeapotAngle2 = 0.0;
static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static GLfloat g_nearPlane = 1;
static GLfloat g_farPlane = 5000;
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

static float g_yawScale = 100.0;
static float g_pitchScale = 100.0;

inline void DrawCubeFace(float fSize)
{
	fSize /= 2.0;
	glBegin(GL_QUADS);
	glVertex3f(-fSize, -fSize, fSize);    glTexCoord2f (0, 0);
	glVertex3f(fSize, -fSize, fSize);     glTexCoord2f (1, 0);
	glVertex3f(fSize, fSize, fSize);      glTexCoord2f (1, 1);
	glVertex3f(-fSize, fSize, fSize);     glTexCoord2f (0, 1);
	glEnd();
}

inline void DrawCubeWithTextureCoords (float fSize)
{
	glPushMatrix();
	DrawCubeFace (fSize);
	glRotatef (90, 1, 0, 0);
	DrawCubeFace (fSize);
	glRotatef (90, 1, 0, 0);
	DrawCubeFace (fSize);
	glRotatef (90, 1, 0, 0);
	DrawCubeFace (fSize);
	glRotatef (90, 0, 1, 0);
	DrawCubeFace (fSize);
	glRotatef (180, 0, 1, 0);
	DrawCubeFace (fSize);
	glPopMatrix();
}

float colorBronzeDiff[4] = { 0.8, 0.6, 0.0, 1.0 };
float colorBronzeSpec[4] = { 1.0, 1.0, 0.4, 1.0 };
float colorBlue[4]       = { 0.0, 0.2, 1.0, 1.0 };
float colorNone[4]       = { 0.0, 0.0, 0.0, 0.0 };


Branches* branches;
static glm::vec3 tree_x0(-5000.0f);
static glm::vec3 tree_x1( 5000.0f);

void		render_branch(Branch & b);

void		render_frame(Frame & f)
{
	for(unsigned int i = 0; i < f.size(); i++) {
		if(f.b(i)->alive == 0) continue;

		glPushMatrix();
		{
			glTranslatef(
					f.b(i)->x[0],
					f.b(i)->x[1],
					f.b(i)->x[2]);


			glutSolidSphere(
					f.b(i)->radius * g_radiusScale,
					resolution,
					resolution);
		}
		glPopMatrix();
	}
}
inline void	RenderObjects2(int t)
{
	Frame & f = u[universe_index]->get_frame(t);

	glColor3f(0.0,1.0,1.0);

	render_frame(f);

	if(g_flags & FLAG::RENDER_BRANCH)
	{	
		//if(t == 0)
		{
			branches->init(f, tree_x0, tree_x1);
		}

		glColor3f(1.0,1.0,1.0);

		for(unsigned int i = 0; i < branches->_M_num_branches; i++)
		{
			Branch & b = branches->get_branch(i);
			render_branch(b);
		}
	}
}
void		render_branch(Branch & b)
{
	if(b._M_flag & KBRANCH_FLAG_IS_LEAF) {
		if(b._M_num_elements > 0) {
			glm::vec3 c = (b._M_x0_glm + b._M_x1_glm) * 0.5f;
			glm::vec3 w = b._M_x1_glm - b._M_x0_glm;

			if(0) {
				print(c);
				print(w);
			}

			glPushMatrix();
			{
				glTranslatef(c.x, c.y, c.z);
				glScalef(w.x, w.y, w.z);
				glutWireCube(1.0);
			}
			glPopMatrix();
		}
	}
}
inline void display()
{
	// Clear frame buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up viewing transformation, looking down -Z axis
	glLoadIdentity();

	//glm::vec3 c = body_center;

	Universe * un = u[universe_index];
	assert(u.size() > universe_index);
	assert(un);
	
	glm::vec3 c = un->mass_center_[ct];

	gluLookAt(
			c.x, c.y, c.z - g_fViewDistance,
			c.x, c.y, c.z,
			0, 1, 0);

	// Set up the stationary light
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);

	glPushMatrix();
	{
		glRotatef(g_yaw / M_PI * 180.0, 0, 1, 0);

		glRotatef(-g_pitch / M_PI * 180.0, cos(-g_yaw), 0, -sin(-g_yaw));

		//glRotatef(g_pitch, 1, 0, 0);

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

		// Global axes
		glBegin(GL_LINES);
		glVertex3f(c.x, c.y, c.z);
		glVertex3f(
				c.x + axes_length,
				c.y,
				c.z);
		glVertex3f(c.x, c.y, c.z);
		glVertex3f(
				c.x,
				c.y + axes_length,
				c.z);
		glVertex3f(c.x, c.y, c.z);
		glVertex3f(
				c.x,
				c.y,
				c.z + axes_length);
		glEnd();
	}
	glPopMatrix();

	// Make sure changes appear onscreen
	glutSwapBuffers();
}
inline void		reset_proj()
{
	g_farPlane = 2.0 * g_fViewDistance;

	glViewport(0, 0, g_Width, g_Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (float)g_Width / g_Height, g_nearPlane, g_farPlane);
	glMatrixMode(GL_MODELVIEW);
}
inline void reshape(GLint width, GLint height)
{
	g_Width = width;
	g_Height = height;

	reset_proj();
}
inline void InitGraphics(void)
{
	//int width, height;
	//int nComponents;
	//void* pTextureImage;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create texture for cube; load marble texture from file and bind it
	/*
	   pTextureImage = read_texture("marble.rgb", &width, &height, &nComponents);
	   glBindTexture(GL_TEXTURE_2D, TEXTURE_ID_CUBE);
	   gluBuild2DMipmaps(GL_TEXTURE_2D,     // texture to specify
	   GL_RGBA,           // internal texture storage format
	   width,             // texture width
	   height,            // texture height
	   GL_RGBA,           // pixel format
	   GL_UNSIGNED_BYTE,	// color component format
	   pTextureImage);    // pointer to texture image

	   glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	   glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	   GL_LINEAR_MIPMAP_LINEAR);
	   glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	   */
}

inline void MouseButton(int button, int state, int x, int y)
{
	// Respond to mouse button presses.
	// If button1 pressed, mark this state so we know in motion function.

	//printf("mouse button = %i, state = %i, x = %i, y = %i\n", button, state, x, y);

	switch(button)
	{
		case GLUT_LEFT_BUTTON:
			g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;

			//g_yClick = y - 3 * g_fViewDistance;

			g_xClick = x - g_yawScale * g_yaw;
			g_yClick = y - g_pitchScale * g_pitch;
			break;
		case 3: // wheel up
			g_fViewDistance *= 0.9;
			printf("g_fViewDistance = %f\n", g_fViewDistance);
			reset_proj();
			break;
		case 4: // wheel down
			g_fViewDistance *= 1.1;
			printf("g_fViewDistance = %f\n", g_fViewDistance);
			reset_proj();
			break;
	}

}

inline void MouseMotion(int x, int y)
{
	// If button1 pressed, zoom in/out if mouse is moved up/down.

	if (g_bButton1Down)
	{
		// view distance
		/*
		   g_fViewDistance = (y - g_yClick) / 3.0;
		   if (g_fViewDistance < VIEWING_DISTANCE_MIN)
		   g_fViewDistance = VIEWING_DISTANCE_MIN;
		   */

		// view angle
		g_yaw = (x - g_xClick) / g_yawScale;
		g_pitch = (y - g_yClick) / g_pitchScale;

		glutPostRedisplay();
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
	g_fTeapotAngle += dt * 30.0;
	g_fTeapotAngle2 += dt * 100.0;

	// Save time_now for next time
	last_idle_time = time_now;

	// Force redraw
	glutPostRedisplay();
}

inline void SelectFromMenu(int idCommand)
{
	switch (idCommand)
	{
		case MENU_LIGHTING:
			g_bLightingEnabled = !g_bLightingEnabled;
			if (g_bLightingEnabled)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
			break;

		case MENU_POLYMODE:
			g_bFillPolygons = !g_bFillPolygons;
			glPolygonMode (GL_FRONT_AND_BACK, g_bFillPolygons ? GL_FILL : GL_LINE);
			break;      

		case MENU_TEXTURING:
			g_bTexture = !g_bTexture;
			if (g_bTexture)
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
			break;    
		case MENU_RESTART:
			ct = 0;
			universe_index = 0;
			break;
		case MENU_EXIT:
			exit (0);
			break;
	}

	// Almost any menu selection requires a redraw
	glutPostRedisplay();
}
inline void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:             // ESCAPE key
			exit (0);
			break;

		case 'l':
			SelectFromMenu(MENU_LIGHTING);
			break;

		case 'p':
			SelectFromMenu(MENU_POLYMODE);
			break;

		case 't':
			SelectFromMenu(MENU_TEXTURING);
			break;
		case 'r':
			SelectFromMenu(MENU_RESTART);
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
inline int BuildPopupMenu (void)
{
	int menu;

	menu = glutCreateMenu(SelectFromMenu);
	glutAddMenuEntry("Toggle lighting\tl", MENU_LIGHTING);
	glutAddMenuEntry("Toggle polygon fill\tp", MENU_POLYMODE);
	glutAddMenuEntry("Toggle texturing\tt", MENU_TEXTURING);
	glutAddMenuEntry("Restart\tr", MENU_TEXTURING);
	glutAddMenuEntry("Exit demo\tEsc", MENU_EXIT);

	return menu;
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
	g_fViewDistance = body_std.x * 10;

	g_farPlane = g_fViewDistance + body_extent.z;

	printf("min:         %f %f %f\n", emin.x, emin.y, emin.z);
	printf("max:         %f %f %f\n", emax.x, emax.y, emax.z);
	printf("mass center: %f %f %f\n", body_extent.x, body_extent.y, body_extent.z);
	printf("std:         %f %f %f\n", body_std.x, body_std.y, body_std.z);


	//u[universe_index]->list(u.num_step-100);

	// GLUT Window Initialization:
	//glutInit (&argc, argv);

	glutInit(&argc,argv);
	glutInitWindowSize (g_Width, g_Height);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("CS248 GLUT example");

	// Initialize OpenGL graphics state
	InitGraphics();

	// Register callbacks:
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (Keyboard);
	glutMouseFunc (MouseButton);
	glutMotionFunc (MouseMotion);
	glutIdleFunc (AnimateScene);

	// Create our popup menu
	BuildPopupMenu ();
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	// Get the initial time, for use by animation
#ifdef _WIN32
	last_idle_time = GetTickCount();
#else
	gettimeofday(&last_idle_time, NULL);
#endif

	// Turn the flow of control over to GLUT
	glutMainLoop();

	return 0;
}




