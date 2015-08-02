
#include "include/kernel/other.hpp"

float			gravity(
				float m0,
				float m1,
				float r,
				float p,
				float v_p)
{
	/* this function actually returns f/r (force/radius)
	 * this allows the resulting scalar to be multiplied each component of r vector to get
	* the correct force components
	*
	* r center distance
	* p penetration
	*/

	// a useful quantity
	float K = GRAV * m0 * m1;

	// simple attraction equation
	float f = K / pow(r,2);

	if(p > 0) {
		// arbitrary repulsion
		//
		// treat repulsion like a spring/damper pair
	
		//rintf("penetration %f\n", p);
	
		// spring
		f -= K * 1000.0 * pow(p,2);
		
		// dramper, apply force opposite the relative velocity along distance vector
		f -= K * 1000.0 * v_p;
	}

	/* return f/r (force/radius)
	* this allows the resulting scalar to be multiplied each component of r vector to get
	* the correct force components
	*/
	return f/r;
}

void			divide(
		unsigned int n,
		unsigned int * i_local0,
		unsigned int * i_local1)
{
	int G = get_global_size(0);

	int g = get_global_id(0);

	unsigned int block = n / G;

	*i_local0 = g * block;
	*i_local1 = *i_local0 + block;

	if(g == (G - 1)) *i_local1 = n;
}

