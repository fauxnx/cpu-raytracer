/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__ 1

#include "hittable.h"
#include "double.h"

class triangle : public hittable {
 public:
  xy_rect() {}
  xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, material *mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};
  virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    box = aabb(vec3(x0,y0, k-0.0001), vec3(x1, y1, k+0.0001));
    return true;
  }
  virtual double pdf_value(const vec4& o, const vec4& v) const {
    hit_record rec;
    if (this->hit(ray(o,v), 0.001, DBL_MAX, rec)) {
      double area = (v1-v0)*(v2-v0)*0.5;
      double distance_squared = rec.t * rec.t * v.squared_length();
      double cosine = fabs(dot(v, rec.normal) / v.length());
      return distance_squared / (cosine * area);
    }
    else
      return 0;
  }
  virtual vec4 random(const vec4& o) const {
    vec4 random_point = vec4(x0 + random_double()*(x1-x0),
                             y0 + random_double()*(y1-y0),
                             k);
    return random_point - o;
  }
  material *mp;
  double v0, v1, v2, k;
};

bool triangle::hit(const ray& r, double t0, double t1, hit_record& rec) const {
  double t = (k-r.origin().z) / r.direction().z;
  if (t < t0 || t > t1)
    return false;
  double x = r.origin().x + t*r.direction().x;
  double y = r.origin().y + t*r.direction().y;
  if (x < x0 || x > x1 || y < y0 || y > y1)
    return false;
  rec.u = (x-x0)/(x1-x0);
  rec.v = (y-y0)/(y1-y0);
  rec.t = t;
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  rec.normal = vec4(0,0,1);
  return true;
}

#endif
