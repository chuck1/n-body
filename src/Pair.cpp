#include <Pair.hpp>

Pair::Pair():
	b0(0),
	b1(0),
	/*u{0,0,0},*/
	/*d(0),*/
	/*f(0),*/
	_M_alive(1),
	_M_collision(0)
{
}
Pair::Pair(Pair const & p)
{
	assert(0);
}
Pair &		Pair::operator=(Pair const & p)
{
	assert(0);
}

