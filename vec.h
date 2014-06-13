#ifndef	_VEC_H_
#define	_VEC_H_

#include <cmath>

namespace edupt {

struct Vec {
	double x, y, z;
	     Vec(const double x = 0, const double y = 0, const double z = 0) : x(x), y(y), z(z) {}
	inline Vec operator+(const Vec &b) const {
		return Vec(x + b.x, y + b.y, z + b.z);
	}
	inline Vec operator-(const Vec &b) const {
		return Vec(x - b.x, y - b.y, z - b.z);
	}
	inline Vec operator*(const double b) const {
		return Vec(x * b, y * b, z * b);
	}
	inline Vec operator/(const double b) const {
		return Vec(x / b, y / b, z / b);
	}
	inline const double length_squared() const { 
		return x*x + y*y + z*z; 
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
	return Vec(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
inline const double dot(const Vec &v1, const Vec &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline const Vec cross(const Vec &v1, const Vec &v2) {
	return Vec(
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x));
}

};

#endif
