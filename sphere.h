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
	Color emission, color;
	ReflectionType reflection_type;

	Sphere(const double radius_, const Vec &position_, const Color &emission_, const Color &color_, const ReflectionType reflection_type_) :
	  radius(radius_), position(position_), emission(emission_), color(color_), reflection_type(reflection_type_) {}

	// 入力のrayに対する交差点までの距離を返す。交差しなかったら0を返す。
	// rayとの交差判定を行う。交差したらtrue,さもなくばfalseを返す。
	bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
		const Vec o_p = position - ray.org;
		const double b = dot(o_p, ray.dir), det = b * b - dot(o_p, o_p) + radius * radius;

		if (det < 0.0)
			return false;
		
		const double sqrt_det = sqrt(det);
		const double t1 = b - sqrt_det, t2 = b + sqrt_det;
	
		if (t1 < kEPS && t2 < kEPS)
			return false;

		if (t1 > kEPS) {
			hitpoint->t = t1;
		} else {
			hitpoint->t = t2;
		}

		hitpoint->position = ray.org + hitpoint->t * ray.dir;
		hitpoint->normal   = normalize(hitpoint->position - position);

		return true;
	}
};

};

#endif