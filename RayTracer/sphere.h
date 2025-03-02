#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
public:
	// Stationary sphere
    sphere(const point3& center, double radius, shared_ptr<material> mat) 
        : center(center, vec3(0,0,0)), radius(std::fmax(0, radius)), mat(mat) 
    {
		auto rvec = vec3(radius, radius, radius);
		bbox = aabb(center - rvec, center + rvec);
    }

	// Moving sphere
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) 
        : center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat) 
    {
		auto rvec = vec3(radius, radius, radius);
        aabb box1(center.at(0) - rvec, center.at(0) + rvec);
		aabb box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = aabb(box1, box2);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		point3 currentCenter = center.at(r.time());
        vec3 oc = currentCenter - r.origin();
        auto a = r.direction().lengthSquared();
        auto h = dot(r.direction(), oc);
        auto c = oc.lengthSquared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (! ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (! ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outwardNormal = (rec.p - currentCenter) / radius;
        rec.set_face_normal(r, outwardNormal);
        rec.mat = mat;

        return true;
    }

	aabb bounding_box() const override {
		return bbox;
	}

private:
    shared_ptr<material> mat;
    ray center;
    double radius;
	aabb bbox;
};

#endif // !SPHERE_H
