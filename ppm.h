#ifndef _PPM_H_
#define _PPM_H_

#include <string>
#include <cstdlib>
#include <vector>

#include "material.h"

namespace edupt {
inline double clamp(double x){ 
	return x<0 ? 0 : x>1 ? 1 : x;
} 
inline int toInt(double x){
	return int(pow(clamp(x), 1/2.2) * 255 + 0.5);
}

void save_ppm_file(const std::string &filename, const Color* image, const int width, const int height) {
	FILE *f = fopen(filename.c_str(), "w"); // Write image to PPM file.
	fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
	for (int i=0; i<width*height; i++)
		fprintf(f,"%d %d %d ", toInt(image[i].x), toInt(image[i].y), toInt(image[i].z));
}


};

#endif