#ifndef VEC3_H
#define VEC3_H

#include "vec3.h"

class vec3 {
public:
	double e[3];


	vec3() : e{ 0, 0, 0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(double i) {
		e[0] *= i;
		e[1] *= i;
		e[2] *= i;
		return *this;
	}

	vec3& operator/=(double i) {
		e[0] /= i;
		e[1] /= i;
		e[2] /= i;
		return *this;
	}

	double length() {
		return std::sqrt(lengthSquared());
	}

	double lengthSquared() const{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	bool nearZero() const {
		auto s = 1e-8;
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
	}
};


using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

inline vec3 random_unit_vector() {
	while (true) {
		auto p = vec3::random(-1, 1);
		auto lensp = p.lengthSquared();
		if (1e-160 < lensp && lensp <= 1) {
			return p / sqrt(lensp);
		}
	}
}

inline vec3 random_on_hemisphere(const vec3& normal) {
	vec3 onUnitSphere = random_unit_vector();
	if (dot(onUnitSphere, normal) > 0.0) return onUnitSphere;
	else return -onUnitSphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cosTheta = std::fmin(dot(-uv, n), 1.0);
	vec3 rOutPerp = etai_over_etat * (uv + cosTheta * n);
	vec3 rOutParalell = -std::sqrt(std::fabs(1.0 - rOutPerp.lengthSquared())) * n;
	return rOutPerp + rOutParalell;
}

inline vec3 random_on_unit_disk() {
	while (true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.lengthSquared() < 1) {
			return p;
		}
	}
}

#endif