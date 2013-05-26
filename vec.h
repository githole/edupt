#ifndef	_VEC_H_
#define	_VEC_H_

namespace edupt {

struct Vec {
	double x, y, z;
	Vec(const double x_ = 0, const double y_ = 0, const double z_ = 0) : x(x_), y(y_), z(z_) {}
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
	inline Vec operator-() {
		return Vec(-x, -y, -z);
	}
	inline const double LengthSquared() const { 
		return x*x + y*y + z*z; 
	}
	inline const double Length() const { 
		return sqrt(LengthSquared()); 
	}
};
inline Vec operator*(double f, const Vec &v) { 
	return v * f; 
}
inline Vec normalize(const Vec &v) {
	return v / v.Length(); 
}
inline const Vec multiply(const Vec &v1, const Vec &v2) {
	return Vec(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
inline const double dot(const Vec &v1, const Vec &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline const Vec cross(const Vec &v1, const Vec &v2) {
	return Vec((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
}

};

#endif