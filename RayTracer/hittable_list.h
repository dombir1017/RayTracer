#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <vector>


class hittable_list : public hittable {
public:
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<hittable> object) { objects.push_back(object); }

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		hit_record tempRec;
		bool hitAnything = false;
		auto closestSoFar = ray_t.max;

		for (const auto& object : objects) {
			if (object->hit(r, interval(ray_t.min, closestSoFar), tempRec)) {
				hitAnything = true;
				closestSoFar = tempRec.t;
				rec = tempRec;
			}
		}

		return hitAnything;
	}
};

#endif

