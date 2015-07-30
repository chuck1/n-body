
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

void		divide(unsigned int n, unsigned int * i_local0, unsigned int * i_local1)
{
	// work group
	int local_block = n / get_num_groups(0);

	int i_group0 = get_group_id(0) * local_block;
	int i_group1 = i_group0 + local_block;

	if(get_group_id(0) == (get_num_groups(0) - 1)) i_group1 = n;

	// work item
	int block = (i_group1 - i_group0) / get_local_size(0);

	*i_local0 = i_group0 + get_local_id(0) * block;
	*i_local1 = *i_local0 + block;

	if(get_local_id(0) == (get_local_size(0) - 1)) *i_local1 = i_group1;
}

