/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __CONSTANT_MEDIUM_H__
#define __CONSTANT_MEDIUM_H__ 1

#include "hittable.h"
#include "material.h"
#include "random.h"
#include "texture.h"

#include <float.h>

class constant_medium : public hittable {
 public:
  constant_medium(hittable *b, double d, texture *a) : boundary(b), density(d) {
    phase_function = new isotropic(a);
  }
  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    return boundary->bounding_box(t0, t1, box);
  }
  hittable *boundary;
  double density;
  material *phase_function;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  hit_record rec1, rec2;

  if (boundary->hit(r, -DBL_MAX, DBL_MAX, rec1)) {
    if (boundary->hit(r, rec1.t+0.0001, DBL_MAX, rec2)) {

      if (rec1.t < t_min)
        rec1.t = t_min;

      if (rec2.t > t_max)
        rec2.t = t_max;

      if (rec1.t >= rec2.t)
        return false;

      if (rec1.t < 0)
        rec1.t = 0;

      double distance_inside_boundary = (rec2.t - rec1.t)*r.direction().length();
      double hit_distance = -(1/density) * log(random_double());

      if (hit_distance < distance_inside_boundary) {

        rec.t = rec1.t + hit_distance / r.direction().length();
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = vec4(1,0,0);
        rec.mat_ptr = phase_function;
        return true;
      }
    }
  }
  return false;
}

#endif
