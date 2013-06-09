#ifndef	_VEC_H_
#define	_VEC_H_

#include <cmath>

namespace edupt {

struct Vec {
	double x_, y_, z_;
	     Vec(const double x = 0, const double y = 0, const double z = 0) : x_(x), y_(y), z_(z) {}
	inline Vec operator+(const Vec &b) const {
		return Vec(x_ + b.x_, y_ + b.y_, z_ + b.z_);
	}
	inline Vec operator-(const Vec &b) const {
		return Vec(x_ - b.x_, y_ - b.y_, z_ - b.z_);
	}
	inline Vec operator*(const double b) const {
		return Vec(x_ * b, y_ * b, z_ * b);
	}
	inline Vec operator/(const double b) const {
		return Vec(x_ / b, y_ / b, z_ / b);
	}
	inline const double length_squared() const { 
		return x_*x_ + y_*y_ + z_*z_; 
	}
	inline const double length() const { 
		return sqrt(length_squared()); 
	}
};
inline Vec operator*(double f, const Vec &v) { 
	return v * f; 
}
inline Vec normalize(const Vec &v) {
	return v * (1.0 / v.length()); 
}
inline const Vec multiply(const Vec &v1, const Vec &v2) {
	return Vec(v1.x_ * v2.x_, v1.y_ * v2.y_, v1.z_ * v2.z_);
}
inline const double dot(const Vec &v1, const Vec &v2) {
	return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
}
inline const Vec cross(const Vec &v1, const Vec &v2) {
	return Vec(
		(v1.y_ * v2.z_) - (v1.z_ * v2.y_),
		(v1.z_ * v2.x_) - (v1.x_ * v2.z_),
		(v1.x_ * v2.y_) - (v1.y_ * v2.x_));
}

};

#endif
