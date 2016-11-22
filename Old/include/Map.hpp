#ifndef MAP_HPP
#define MAP_HPP


struct Map
{
	void			alloc(unsigned int n)
	{
		pair_ = new unsigned int[n*n];
	}
	unsigned int *		ptr()
	{
		return pair_;
	}
	unsigned int *		pair_;
};


#endif


