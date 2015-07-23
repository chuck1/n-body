
#include "include/kernel/kCollisionBuffer.hpp"
#include "include/kernel.hpp"
//#include <kBranches.hpp>
#include "include/kernel/kBody.hpp"
#include "include/kernel/kBranches.hpp"
//#include <free.hpp>
//#include <config.hpp>
//#include <condition_variable>

#define GRAV (6.67384E-11)

#define DEBUG (0)

float	vec_dot(
		float * a,
		float * b)
{
	float d = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	return d;
}
void	vec_sub(
		float * a,
		float * b,
		float * c)
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}
void	vec_sub_prod(
		float * a,
		float * b,
		float c)
{
	a[0] -= b[0] * c;
	a[1] -= b[1] * c;
	a[2] -= b[2] * c;
}
void	vec_add_prod(
		float * a,
		float * b,
		float c)
{
	a[0] += b[0] * c;
	a[1] += b[1] * c;
	a[2] += b[2] * c;
}
float	vec_length(float * a)
{
	return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}
void	mark_collision(
	       struct kCollisionBuffer * cb,
	       unsigned int body_idx_0,
	       unsigned int body_idx_1,
	       struct kBody * pb0,
	       struct kBody * pb1)
{
	if(0)
	{

	// atomic
	{
#if THREADED
		std::lock_guard<std::mutex> lock(cb->_M_mutex);
#endif
	//rintf("collision\n");
	//Pair & pair = pairs[map_func(body_idx_0, body_idx_1, num_bodies)];

	//ssert(cb->_M_size < kCollisionBuffer::LENGTH);

		cb->_M_pairs[cb->_M_size].i = body_idx_0;
		cb->_M_pairs[cb->_M_size].j = body_idx_1;
		cb->_M_pairs[cb->_M_size].flag |= KCOLL_PAIR_FLAG_UNRESOLVED;
		cb->_M_size++;
	}

	//pair._M_collision = 1;

	{
	#if THREADED
	std::lock_guard<std::mutex> lock(g_mutex_bodies);
	#endif

	// atomic
	pb0->num_collisions++;
	// atomic
	pb1->num_collisions++;
	}

	}

	float v_rel[3];
	vec_sub(v_rel, pb1->v, pb0->v);
	
	float c = 10.0;

	pb0->f[0] += c * v_rel[0];
	pb0->f[1] += c * v_rel[1];
	pb0->f[2] += c * v_rel[2];
	
	pb1->f[0] -= c * v_rel[0];
	pb1->f[1] -= c * v_rel[1];
	pb1->f[2] -= c * v_rel[2];

}

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
void			step_pairs_in_branch(
		     struct kBranch * branch,
		     struct kCollisionBuffer * cb,
		     struct kBody * bodies)
{
// for each pair of bodies in the branch
for(unsigned int j = 0; j < branch->_M_num_elements; j++)
	     {
	     for(unsigned int k = j + 1; k < branch->_M_num_elements; k++)
			  {
			  unsigned int body_idx_0 = branch->_M_elements[j];
			  unsigned int body_idx_1 = branch->_M_elements[k];

			  struct kBody * pb0 = bodies + body_idx_0;
			  struct kBody * pb1 = bodies + body_idx_1;

			  //ssert(pb0 != pb1);
			  float D[3];
			  vec_sub(D, pb0->x, pb1->x);

			  float len_D = vec_length(D);

			  // radius sum
			  float RS = pb0->radius + pb1->radius;
			  // penetration (positive means penetrating)
			  float pen = RS - len_D;
			  // relative velocity of body 1 wrt body 0 (body 0 stationary)
			  float v[3];
			  vec_sub(v, pb1->v, pb0->v);
			  // relative speed along positive p_vector (moving toward each other is positive)
			  float v_p = vec_dot(v,D) / len_D;

			  float f = gravity(pb0->mass, pb1->mass, len_D, pen, v_p);

			  //if(DEBUG) rintf("intra-branch f = %f\n", f);

			  {
			  #if THREADED
			  //std::lock_guard<std::mutex> lock(g_mutex_bodies);
			  #endif
			  vec_sub_prod(pb0->f, D, f);
			  vec_add_prod(pb1->f, D, f);
			  }

			  if(len_D < (pb0->radius + pb1->radius))
			  {
			  mark_collision(cb, body_idx_0, body_idx_1, pb0, pb1);
			  }
			  }
			  }
			  }
bool			branch_pair_should_calc(
		__global struct kBranch* b0,
		__global struct kBranch* b1)
{
	if(!(b0->_M_flag & KBRANCH_FLAG_IS_LEAF)) {
		return false;
	} if(!(b1->_M_flag & KBRANCH_FLAG_IS_LEAF)) {
		return false;
	}
	if(b0->_M_num_elements == 0) {
		return false;
	}
	if(b1->_M_num_elements == 0) {
		return false;
	}
			  return true;
			  }
void			step_branch_pairs(
		__global struct kBranches * branches,
		__global struct kCollisionBuffer * cb,
		__global struct kBody * bodies
								  /*
								  Pair * pairs,
								  unsigned int * map,
								  unsigned int num_bodies
								  */
		)
{
	unsigned int count = 0;
	unsigned int count_body_body = 0;
	unsigned int count_body_branch = 0;

	unsigned int i_local0;
	unsigned int i_local1;

	//divide(branches->_M_num_branch_pairs, i_local0, i_local1);
	divide(branches->_M_num_branches, &i_local0, &i_local1);

	//if(DEBUG) rintf("branch pairs %6i to %6i\n", i_local0, i_local1);

	/* compute */
	for(unsigned int i = i_local0; i < i_local1; i++) {
		for(unsigned int j = i + 1; j < branches->_M_num_branches; j++) {
			//struct BranchPair* pp = branches->_M_branch_pairs + p;
			//struct Branch* b0 = branches->_M_branches + pp->b0;
			//struct Branch* b1 = branches->_M_branches + pp->b1;

			__global struct kBranch* b0 = branches->_M_branches + i;
			__global struct kBranch* b1 = branches->_M_branches + j;

			if(!branch_pair_should_calc(b0, b1)) continue;

			//__local float * x0 = b0->x;
			//__local float * x1 = b1->x;
			float * x0 = b0->_M_mc;
			float * x1 = b1->_M_mc;

			float r[3];
			vec_sub(r, x0, x1);
			float d = vec_length(r);

			count++;

			// rough size of the box
			/*
			lm::vec3 W0 = b0->_M_x1_lm - b0->_M_x0_lm;
			lm::vec3 W1 = b1->_M_x1_lm - b1->_M_x0_lm;

			float w0 = lm::length(W0); 
			float w1 = lm::length(W1); 
			*/
			float ratio = 0.3;

			if(
			  (d > b0->_M_width) &&
			  (d > b1->_M_width) &&
			  ((b1->_M_width / (d - b0->_M_width)) < ratio) &&
			  ((b0->_M_width / (d - b1->_M_width)) < ratio)
			) {
				//if(DEBUG) rintf("w0 = %f\nw1 = %f\nd  = %f\n", b0->_M_width, b1->_M_width, d);

				count_body_branch++;

				//use branch 1 mass for bodies in branch 0

				if(0) { // per body displacement vector
				// for each body in branch 0
				for(unsigned int i = 0; i < b0->_M_num_elements; i++) {
					 struct kBody * pb = bodies + b0->_M_elements[i];

					float D[3];
					vec_sub(D, b1->_M_mc, pb->x);
					float len_D = vec_length(D);

					float f = gravity(pb->mass, b1->_M_mass, len_D, -1, 0);

					//ssert(b1->_M_mass > 0);
					//if(DEBUG) rintf("inter-branch body branch1 f = %f\n", f);

					pb->f[0] += f * D[0];
					pb->f[1] += f * D[1];
					pb->f[2] += f * D[2];

					count++;
			}
			// for each body in branch 0
			for(unsigned int i = 0; i < b1->_M_num_elements; i++) {
				struct kBody * pb = bodies + b1->_M_elements[i];
					
				float D[3];
				vec_sub(D, b0->_M_mc, pb->x);
				float len_D = vec_length(D);

				float f = gravity(pb->mass, b0->_M_mass, len_D, -1, 0);

				//ssert(b0->_M_mass > 0);

				//if(DEBUG) rintf("inter-branch body branch0 f = %f\n", f);

				pb->f[0] += f * D[0];
				pb->f[1] += f * D[1];
				pb->f[2] += f * D[2];

				count++;
			}
		} else { // use same displacement vector for all
			// for each body in branch 0
			for(unsigned int i = 0; i < b0->_M_num_elements; i++)  {
				struct kBody * pb = bodies + b0->_M_elements[i];

				float f0 = gravity(pb->mass, b1->_M_mass, d, -1, 0);

				//ssert(b1->_M_mass > 0);

				//if(DEBUG) rintf("inter-branch body branch1 f = %f\n", f0);

				pb->f[0] += f0 * r[0];
				pb->f[1] += f0 * r[1];
				pb->f[2] += f0 * r[2];
			}
			// for each body in branch 0
			for(unsigned int i = 0; i < b1->_M_num_elements; i++) {
				struct kBody * pb = bodies + b1->_M_elements[i];
				float f1 = gravity(pb->mass, b0->_M_mass, d, -1, 0);
				//ssert(b0->_M_mass > 0);

				//if(DEBUG) rintf("inter-branch body branch0 f = %f\n", f1);

				pb->f[0] += f1 * r[0];
				pb->f[1] += f1 * r[1];
				pb->f[2] += f1 * r[2];
			}
}
} else {
	count_body_body++;
	// for each body in branch 0
	for(unsigned int i = 0; i < b0->_M_num_elements; i++) {
		// for each body in branch 0
		for(unsigned int j = 0; j < b1->_M_num_elements; j++)   {
			unsigned int body_idx_0 = b0->_M_elements[i];
			unsigned int body_idx_1 = b1->_M_elements[j];

			struct kBody * pb0 = bodies + body_idx_0;
			struct kBody * pb1 = bodies + body_idx_1;

			//ssert(pb0 != pb1);

			// vecotr pointing toward body 0
			float D[3];
			vec_sub(D, pb0->x, pb1->x);
			float len_D = vec_length(D);
		
			// radius sum
			float RS = pb0->radius + pb1->radius;
			// penetration (positive means penetrating)
			float pen = RS - len_D;
			// relative velocity of body 1 wrt body 0 (body 0 stationary)
			float v[3];
			vec_sub(v, pb1->v, pb0->v);
			// relative speed along positive p_vector (moving toward each other is positive)
			float v_p = vec_dot(v,D) / len_D;
		
			float f = gravity(pb0->mass, pb1->mass, len_D, pen, v_p);
		
			//if(DEBUG) rintf("inter-branch body-body f = %f\n", f);
		
			pb0->f[0] -= f * D[0];
			pb0->f[1] -= f * D[1];
			pb0->f[2] -= f * D[2];

			pb1->f[0] += f * D[0];
			pb1->f[1] += f * D[1];
			pb1->f[2] += f * D[2];
		
			if(len_D < (pb0->radius + pb1->radius)) {
				mark_collision(cb, body_idx_0, body_idx_1, pb0, pb1);
			}

			count++;
		}
	}
	}
	}
	}

	divide(branches->_M_num_branches, &i_local0, &i_local1);
	//if(DEBUG) rintf("branches %6i to %6i\n", i_local0, i_local1);

	// also must calc forces between bodies in same branch!
	for(unsigned int i = i_local0; i < i_local1; i++) {
		step_pairs_in_branch(&(branches->_M_branches[i]), cb, bodies);
	}

	//if(DEBUG) rintf(
	//	       "count = %i count_body_body = %i count_body_branch = %i\n",
	//		count, count_body_body, count_body_branch);
}



