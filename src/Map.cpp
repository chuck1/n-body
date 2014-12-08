#include <Map.hpp>

Map::Map(): pair_(0)
{
}
Map::Map(Map&& m): pair_(std::move(m.pair_))
{
}
Map &			Map::operator=(Map const & m)
{
	return *this;
}
void			Map::alloc(unsigned int n)
{
	//printf("%s\n", __PRETTY_FUNCTION__);
	//printf("%i %i\n", n, (int)(sizeof(unsigned int) * n * n));

	pair_.resize(n*n);
}
unsigned int *		Map::ptr()
{
	return &pair_[0];
}


