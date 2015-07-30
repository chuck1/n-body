#ifndef K_COLLISION_BUFFER_HPP
#define K_COLLISION_BUFFER_HPP

#define KCOLL_PAIR_FLAG_UNRESOLVED (1 << 0);

struct kCollisionBuffer
{
	enum
	{
		LENGTH = 1000,
	};
	struct kPair
	{
		unsigned int	i;
		unsigned int	j;
		unsigned int	flag;
	};

	struct kPair	_M_pairs[LENGTH];
	unsigned int	_M_size;

};

#endif
