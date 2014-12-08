
#include <decl.hpp>

int	get_num_groups(int);
int	get_group_id(int);
int	get_local_id(int);
int	get_local_size(int);
int	get_global_size(int);
float	rsqrt(float);

void step_bodies(
		struct Body * bodies,
		/*struct Pair * pairs,*/
		/*unsigned int * map,*/
		float dt,
		unsigned int num_bodies,
		float * velocity_ratio,
		float * mass_center,
		float mass,
		unsigned int * number_escaped
		);

void step_pairs(
		struct Body * bodies,
		struct Pair * pairs,
		unsigned int num_pairs
	       );

void			step_collisions(
		struct Body* bodies, /* readonly */
		CollisionBuffer * cb,
		/*struct Pair* pairs,*/
		unsigned int * flag_multi_coll,
		unsigned int * nc
		/*unsigned int num_bodies*/
		);
void			clear_bodies_num_collisions(
		struct Body * bodies,
		unsigned int num_bodies
		);
void			update_branches(
		Branches * branches,
		Body * bodies
		);
void			step_branch_pairs(
		Branches * branches,
		CollisionBuffer * cb,
		Body * bodies
		);




