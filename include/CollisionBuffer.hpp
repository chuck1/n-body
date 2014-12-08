#ifndef COLLISION_BUFFER_HPP
#define COLLISION_BUFFER_HPP

class CollisionBuffer
{
	public:
		enum
		{
			LENGTH = 10000,
		};
		class Pair
		{
			public:
				enum
				{
					FLAG_UNRESOLVED = 1 << 0,
				};
				unsigned int	i;
				unsigned int	j;
				unsigned int	flag;
		};
		
		CollisionBuffer(): _M_size(0) {}
		
		Pair		_M_pairs[LENGTH];
		unsigned int	_M_size;
};

#endif
