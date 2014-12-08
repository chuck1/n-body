#include <universe.h>

void			Pairs::init(Frame const & f)
{
	assert(f.size() > 0);

	int k = 0;

	unsigned int nb = f.size();



	//map_.alloc(nb);

	pairs_.resize(nb * (nb - 1) / 2);

	for(unsigned int i = 0; i < nb; i++)
	{
		for(unsigned int j = i + 1; j < nb; j++)
		{

			pairs_[k].b0 = i;
			pairs_[k].b1 = j;

			/*
			map_.pair_[i * nb + j] = k;
			map_.pair_[j * nb + i] = k;
		*	*/
			k++;
		}
	}
}
unsigned int		Pairs::size() const
{
	return pairs_.size();
}



