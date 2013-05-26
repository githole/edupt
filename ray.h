#ifndef _RAY_H_
#define _RAY_H_

#include "vec.h"

namespace edupt {

struct Ray {
	Vec org, dir;
	Ray(const Vec &org_, const Vec &dir_) : org(org_), dir(dir_) {}
};

};

#endif _RAY_H_