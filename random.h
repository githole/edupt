#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <limits>

namespace edupt {

class XorShift {
	unsigned int seed[4];
public:
	unsigned int next(void) { 
		unsigned int t = seed[0] ^ (seed[0] << 11);
		seed[0] = seed[1]; 
		seed[1] = seed[2];
		seed[2] = seed[3];
		return seed[3] = (seed[3] ^ (seed[3] >> 19)) ^ (t ^ (t >> 8)); 
	}

	double next01(void) {
		return (double)next() / UINT_MAX;
	}

	XorShift(const unsigned int initial_seed) {
		unsigned int s = initial_seed;
		for(int i=1; i<=4; i++){
			seed[i-1] = s = 1812433253U * (s^(s>>30)) + i;
		}
	}
};

typedef XorShift Random;

};

#endif