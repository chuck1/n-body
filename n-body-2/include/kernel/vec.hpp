
bool	vec_all_greater_than(
		float * a,
		float * b);
bool	vec_all_greater_than_eq(
		float * a,
		float * b);
bool	vec_all_less_than(
		float * a,
		float * b);
bool	vec_all_greater_than_2(
		__global float const * a,
		__global float const * b);
bool	vec_all_greater_than_eq_2(
		__global float const * a,
		__global float const * b);
bool	vec_all_less_than_2(
		__global float const * a,
		__global float const * b);
float	vec_dot(
		float * a,
		float * b);
void	vec_sub(
		float * a,
		float * b,
		float * c);
void	vec_sub_2(
		float * a,
		__local float * b,
		__local float * c);
void	vec_sub_3(
		float * a,
		__global float * b,
		__global float * c);
void	vec_sub_prod(
		__global float * a,
		float * b,
		float c);
void	vec_add_prod(
		__global float * a,
		float * b,
		float c);
float	vec_length(float * a);

