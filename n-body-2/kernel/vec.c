
bool	vec_all_greater_than(
		float * a,
		float * b)
{
	if(a[0] <= b[0]) return false;
	if(a[1] <= b[1]) return false;
	if(a[2] <= b[2]) return false;
	return true;
}
bool	vec_all_less_than(
		float * a,
		float * b)
{
	if(a[0] >= b[0]) return false;
	if(a[1] >= b[1]) return false;
	if(a[2] >= b[2]) return false;
	return true;
}
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
	a[0] = b[0] - c[0];
	a[1] = b[1] - c[1];
	a[2] = b[2] - c[2];
}
void	vec_sub_2(
		float * a,
		__local float * b,
		__local float * c)
{
	a[0] = b[0] - c[0];
	a[1] = b[1] - c[1];
	a[2] = b[2] - c[2];
}
void	vec_sub_3(
		float * a,
		__global float * b,
		__global float * c)
{
	a[0] = b[0] - c[0];
	a[1] = b[1] - c[1];
	a[2] = b[2] - c[2];
}
void	vec_sub_prod(
		__global float * a,
		float * b,
		float c)
{
	a[0] -= b[0] * c;
	a[1] -= b[1] * c;
	a[2] -= b[2] * c;
}
void	vec_add_prod(
		__global float * a,
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

