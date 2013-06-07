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
	double radius_;
	Vec position_;
	Color emission_;
	Color color_;
	ReflectionType reflection_type_;

	Sphere(const double radius, const Vec &position, const Color &emission, const Color &color, const ReflectionType reflection_type) :
	  radius_(radius), position_(position), emission_(emission), color_(color), reflection_type_(reflection_type) {}

	// 入力のrayに対する交差点までの距離を返す。交差しなかったら0を返す。
	// rayとの交差判定を行う。交差したらtrue,さもなくばfalseを返す。
	bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
		const Vec o_p = position_ - ray.org_;
		const double b = dot(o_p, ray.dir_);
		const double det = b * b - dot(o_p, o_p) + radius_ * radius_;

		if (det < 0.0)
			return false;
		
		const double sqrt_det = sqrt(det);
		const double t1 = b - sqrt_det, t2 = b + sqrt_det;
	
		if (t1 < kEPS && t2 < kEPS)
			return false;

		if (t1 > kEPS) {
			hitpoint->distance_ = t1;
		} else {
			hitpoint->distance_ = t2;
		}

		hitpoint->position_ = ray.org_ + hitpoint->distance_ * ray.dir_;
		hitpoint->normal_   = normalize(hitpoint->position_ - position_);
		return true;
	}
};

};

#endif
