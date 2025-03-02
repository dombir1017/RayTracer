#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& rIn, const hit_record& rec, color& attenuation, ray& scattered) const {
		return false;
	}
};

class lambertian : public material {
public:
	lambertian(const color& albedo) : albedo(albedo) {}

	bool scatter(const ray& rIn, const hit_record& rec, color& attenuation, ray& scattered) const override {
		auto scatterDirection = rec.normal + random_unit_vector();
		if (scatterDirection.nearZero()) scatterDirection = rec.normal;

		scattered = ray(rec.p, scatterDirection, rIn.time());
		attenuation = albedo;
		return true;
	}

private:
	color albedo;
};

class metal : public material {
public:
	metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

	bool scatter(const ray& rIn, const hit_record& rec, color& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(rIn.direction(), rec.normal);
		reflected = unit_vector(reflected) + fuzz * random_unit_vector();
		scattered = ray(rec.p, reflected, rIn.time());
		attenuation = albedo;
		return dot(scattered.direction(), rec.normal) > 0;
	}

private:
	color albedo;
	double fuzz;
};

class dielectric : public material {
public:
	dielectric(double refractiveIndex) : refractiveIndex(refractiveIndex) {}

	bool scatter(const ray& rIn, const hit_record& rec, color& attenuation, ray& scattered) const override {
		attenuation = color(1.0, 1.0, 1.0);
		double ri = rec.frontFace ? (1.0 / refractiveIndex) : refractiveIndex;

		vec3 unitDirection = unit_vector(rIn.direction());

		double cosTheta = std::fmin(dot(-unitDirection, rec.normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

		vec3 direction;

		if (ri * sinTheta > 1.0 || reflectance(cosTheta, ri) > random_double()) {
			direction = reflect(unitDirection, rec.normal);
		}
		else {
			direction = refract(unitDirection, rec.normal, ri);
		}
		scattered = ray(rec.p, direction, rIn.time());
		return true;
	}

private:
	double refractiveIndex;

	static double reflectance(double cosine, double refractiveIndex) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};


#endif
