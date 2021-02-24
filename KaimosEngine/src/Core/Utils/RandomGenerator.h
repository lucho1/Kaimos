#ifndef _RANDOMGENERATOR_H_
#define _RANDOMGENERATOR_H_

#include "Core/Core.h"
#include <random>

namespace Kaimos::Random
{
	static std::random_device s_Seed;
	static std::default_random_engine s_Engine = std::default_random_engine(s_Seed());

	static std::uniform_int_distribution<int> s_IntDistribution;
	static std::uniform_real_distribution<double> s_DoubleDistribution;

	// Returns a random int in range (including both)
	// If both values are 0, it will return an un-ranged random int
	static int GetRandomInt(int range_min = 0, int range_max = 0)
	{
		if (range_min == 0 && range_max == 0)
			return s_IntDistribution(s_Engine);

		KS_ENGINE_ASSERT(range_min < range_max, "RangeMin has to be lower than RangeMax");
		std::uniform_int_distribution<int> dist(range_min, range_max);
		return dist(s_Engine);
	}

	// Returns a random vector of ints in range (including both)
	// If both values are 0, it will return an un-ranged random int vector
	static std::vector<int> GetRandomIntVector(int vec_size, int range_min = 0, int range_max = 0)
	{
		std::vector<int> ret;
		std::uniform_int_distribution<int>* dist;

		if (range_min == 0 && range_max == 0)
		{
			std::uniform_int_distribution<int> d;
			dist = &d;
		}
		else
		{
			std::uniform_int_distribution<int> d(range_min, range_max);
			dist = &d;
		}

		for (int i = 0; i < vec_size; ++i)
			ret.push_back((*dist)(s_Engine));

		return ret;
	}

	// Returns a random float in range (including both)
	// If both values are 0, it will return a random float,
	// but between 0 and 1, so a multiplication might be needed
	static float GetRandomFloat(int range_min = 0, int range_max = 0)
	{
		if (range_min == 0 && range_max == 0)
			return (float)s_DoubleDistribution(s_Engine);

		KS_ENGINE_ASSERT(range_min < range_max, "RangeMin has to be lower than RangeMax");
		std::uniform_real_distribution<double> dist((double)range_min, (double)range_max);
		return (float)dist(s_Engine);
	}

	// Returns a random vector of floats in range (including both)
	// If both values are 0, it will return a random float vector,
	// but between 0 and 1, so a multiplication might be needed
	static std::vector<float> GetRandomFloatVector(int vec_size, int range_min = 0, int range_max = 0)
	{
		std::vector<float> ret;
		std::uniform_real_distribution<double>* dist;

		if (range_min == 0 && range_max == 0)
		{
			std::uniform_real_distribution<double> d;
			dist = &d;
		}
		else
		{
			std::uniform_real_distribution<double> d((double)range_min, (double)range_max);
			dist = &d;
		}

		for (int i = 0; i < vec_size; ++i)
			ret.push_back((float)(*dist)(s_Engine));

		return ret;
	}
}
#endif //_RANDOMGENERATOR_H_
