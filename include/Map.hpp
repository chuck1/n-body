#ifndef MAP_HPP
#define MAP_HPP


struct Map
{
	Map(): pair_(0)
	{
	}
	void			alloc(unsigned int n)
	{
		if(pair_)
		{
			delete[] pair_;
		}
		pair_ = new unsigned int[n*n];
	}
	unsigned int *		ptr()
	{
		return pair_;
	}
	unsigned int *		pair_;
};


#endif


