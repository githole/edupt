#ifndef	_SPHERE_H_
#define	_SPHERE_H_

#include <cmath>

#include "vec.h"
#include "ray.h"
#include "material.h"
#include "constant.h"
#include "intersection.h"

namespace edupt {

struct Sphere {
	double radius;
	Vec position;
	Color emission;
	Color color;
	ReflectionType reflection_type;

	Sphere(const double radius, const Vec &position, const Color &emission, const Color &color, const ReflectionType reflection_type) :
	  radius(radius), position(position), emission(emission), color(color), reflection_type(reflection_type) {}

	// 入力のrayに対する交差点までの距離を返す。交差しなかったら0を返す。
	// rayとの交差判定を行う。交差したらtrue,さもなくばfalseを返す。
	bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
		const Vec p_o = position - ray.org;
		const double b = dot(p_o, ray.dir);
		const double D4 = b * b - dot(p_o, p_o) + radius * radius;

		if (D4 < 0.0)
			return false;
		
		const double sqrt_D4 = sqrt(D4);
		const double t1 = b - sqrt_D4, t2 = b + sqrt_D4;
	
		if (t1 < kEPS && t2 < kEPS)
			return false;

		if (t1 > kEPS) {
			hitpoint->distance = t1;
		} else {
			hitpoint->distance = t2;
		}

		hitpoint->position = ray.org + hitpoint->distance * ray.dir;
		hitpoint->normal   = normalize(hitpoint->position - position);
		return true;
	}
};

};

#endif
