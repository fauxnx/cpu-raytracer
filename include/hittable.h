/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __HITTABLE_H__
#define __HITTABLE_H__ 1

#include "ray.h"
#include "aabb.h"

class material;

struct hit_record {
  double t;
  double u;
  double v;
  vec4 p;
  vec4 normal;
  material *mat_ptr;
};

class hittable {
 public:
  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
  virtual bool bounding_box(double t0, double t1, aabb& box) const = 0;
  virtual double pdf_value(const vec4& o, const vec4& v) const  { return 0.0; }
  virtual vec4 random(const vec4& o) const { return vec4(1, 0, 0); }
};

class flip_normals : public hittable {
 public:
  flip_normals(hittable *p) : ptr(p) {}
  virtual bool hit( const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (ptr->hit(r, t_min, t_max, rec)) {
      rec.normal = -rec.normal;
      return true;
    }
    else
      return false;
  }
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    return ptr->bounding_box(t0, t1, box);
  }
  hittable *ptr;
};

class translate : public hittable {
 public:
  translate(hittable *p, const vec4& displacement)
    : ptr(p), offset(displacement) {}
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const;
    hittable *ptr;
    vec4 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  ray moved_r(r.origin() - offset, r.direction(), r.time());
  if (ptr->hit(moved_r, t_min, t_max, rec)) {
    rec.p += offset;
    return true;
  }
  else
    return false;
}

bool translate::bounding_box(double t0, double t1, aabb& box) const {
  if (ptr->bounding_box(t0, t1, box)) {
    box = aabb(box.min() + offset, box.max() + offset);
    return true;
  }
  else
    return false;
}

class rotate_y : public hittable {
  public:
    rotate_y(hittable *p, double angle);
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
      box = bbox; return hasbox;
    }
    hittable *ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;
};

rotate_y::rotate_y(hittable *p, double angle) : ptr(p) {
  double radians = (static_cast<double>(M_PI) / 180.0) * angle;
  sin_theta = sin(radians);
  cos_theta = cos(radians);
  hasbox = ptr->bounding_box(0, 1, bbox);
  vec4 min(DBL_MAX, DBL_MAX, DBL_MAX);
  vec4 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        double x = i*bbox.max().x + (1-i)*bbox.min().x;
        double y = j*bbox.max().y + (1-j)*bbox.min().y;
        double z = k*bbox.max().z + (1-k)*bbox.min().z;
        double newx = cos_theta*x + sin_theta*z;
        double newz = -sin_theta*x + cos_theta*z;
        vec4 tester(newx, y, newz);
        for (int c = 0; c < 3; c++) {
          if (tester[c] > max[c])
            max[c] = tester[c];
          if (tester[c] < min[c])
            min[c] = tester[c];
        }
      }
    }
  }
  bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  vec4 origin = r.origin();
  vec4 direction = r.direction();
  origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
  origin[2] =  sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
  direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
  direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
  ray rotated_r(origin, direction, r.time());
  if (ptr->hit(rotated_r, t_min, t_max, rec)) {
    vec4 p = rec.p;
    vec4 normal = rec.normal;
    p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];
    normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];
    rec.p = p;
    rec.normal = normal;
    return true;
  }
  else
    return false;
}
#endif
