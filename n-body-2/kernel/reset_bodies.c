
#include "include/kernel/kBody.hpp"
#include "include/kernel/kDebug.hpp"
#include "include/kernel/other.hpp"

__kernel void			reset_bodies(
		__global struct kBody * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		unsigned int num_bodies,
		__global struct kDebug * db
		//float * velocity_ratio, // in/out
		//float * mass_center, // in
		//float mass, // in
		//unsigned int * number_escaped // out
		)
{
	unsigned int i_local0;
	unsigned int i_local1;
	
	divide(num_bodies, &i_local0, &i_local1);

	// debug info
	int i = get_local_id(0) + get_local_size(0) * get_num_groups(0);
	db->_M_i_local_0[i] = i_local0;
	db->_M_i_local_1[i] = i_local1;
	db->_M_n = num_bodies;
	db->_M_global_size = get_num_groups(0);
	db->_M_local_size = get_local_size(0);
	

	__global struct kBody * pb = 0;

	for(unsigned int b = i_local0; b < i_local1; b++) {

		pb = bodies + b;

		if(pb->alive == 0) continue;

		// reset accumulating force
		pb->f[0] = 0;
		pb->f[1] = 0;
		pb->f[2] = 0;
	}
}







