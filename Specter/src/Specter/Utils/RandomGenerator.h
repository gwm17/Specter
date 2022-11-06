/*
	RandomGenerator.h
	Thread-safe implementation of stl random number generator library. A single Mersenne-Twister generator is created for
	each thread. The paramters for the distributions are passed in to the function call, and a random number is calculated
	for the requested distribution. Note that distributions are templated so that one can specify the type of distributions,
	but most distributions only support a limited range of types. The valid types are commented for each case.

	Oct 2022 -- GWM
*/

#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>

namespace Specter {

	class RandomGenerator
	{
	public:

		//Valid for float, double, long double
		template<typename T>
		static T GetUniformReal(T min, T max)
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(GetGenerator());
		}

		//Valid for any integer type (signed or unsigned)
		template<typename T>
		static T GetUniformInt(T min, T max)
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(GetGenerator());
		}

		//Valid for float, double, long double
		template<typename T>
		static T GetNormal(T mean, T sigma)
		{
			std::normal_distribution<T> distribution(mean, sigma);
			return distribution(GetGenerator());
		}

		//This is the most common use case, so we eliminate recreation of distribution, templating. 
		//For randomization of decimals in conversion from integer -> floating point for histograming
		static double GetUniformFraction()
		{
			static std::uniform_real_distribution<double> distribution(0.0, 1.0);
			return distribution(GetGenerator());
		}

	private:
		static std::mt19937_64& GetGenerator()
		{
			static thread_local std::mt19937_64 generator((std::random_device())());
			return generator;
		}
	};
}


#endif