/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __AARECT_H__
#define __AARECT_H__ 1

#include "hittable.h"
#include "float.h"

class xy_rect : public hittable {
 public:
  xy_rect() {}
  xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, material *mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};
  virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    box = aabb(vec4(x0,y0, k-0.0001), vec4(x1, y1, k+0.0001));
    return true;
  }
  virtual double pdf_value(const vec4& o, const vec4& v) const {
    hit_record rec;
    if (this->hit(ray(o,v), 0.001, DBL_MAX, rec)) {
      double area = (x1-x0)*(y1-y0);
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
  double x0, x1, y0, y1, k;
};

bool xy_rect::hit(const ray& r, double t0, double t1, hit_record& rec) const {
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

class xz_rect: public hittable {
 public:
  xz_rect() {}
  xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, material *mat)
    : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
  virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    box =  aabb(vec4(x0,k-0.0001,z0), vec4(x1, k+0.0001, z1));
    return true;
  }
  virtual double pdf_value(const vec4& o, const vec4& v) const {
    hit_record rec;
    if (this->hit(ray(o,v), 0.001, DBL_MAX, rec)) {
      double area = (x1-x0)*(z1-z0);
      double distance_squared = rec.t * rec.t * v.squared_length();
      double cosine = fabs(dot(v, rec.normal) / v.length());
      return distance_squared / (cosine * area);
    }
    else
      return 0;
  }
  virtual vec4 random(const vec4& o) const {
    vec4 random_point = vec4(x0 + random_double()*(x1-x0),
                             k,
                             z0 + random_double()*(z1-z0));
    return random_point - o;
  }
  material *mp;
  double x0, x1, z0, z1, k;
};

bool xz_rect::hit(const ray& r, double t0, double t1, hit_record& rec) const {
  double t = (k-r.origin().y) / r.direction().y;
  if (t < t0 || t > t1)
    return false;
  double x = r.origin().x + t*r.direction().x;
  double z = r.origin().z + t*r.direction().z;
  if (x < x0 || x > x1 || z < z0 || z > z1)
    return false;
  rec.u = (x-x0)/(x1-x0);
  rec.v = (z-z0)/(z1-z0);
  rec.t = t;
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  rec.normal = vec4(0, 1, 0);
  return true;
}

class yz_rect: public hittable {
 public:
  yz_rect() {}
  yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, material *mat)
    : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
  virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    box =  aabb(vec4(k-0.0001, y0, z0), vec4(k+0.0001, y1, z1));
    return true;
  }
  virtual double pdf_value(const vec4& o, const vec4& v) const {
    hit_record rec;
    if (this->hit(ray(o,v), 0.001, DBL_MAX, rec)) {
      double area = (y1-y0)*(z1-z0);
      double distance_squared = rec.t * rec.t * v.squared_length();
      double cosine = fabs(dot(v, rec.normal) / v.length());
      return distance_squared / (cosine * area);
    }
    else
      return 0;
  }
  virtual vec4 random(const vec4& o) const {
    vec4 random_point = vec4(k,
                             y0 + random_double()*(y1-y0),
                             z0 + random_double()*(z1-z0));
    return random_point - o;
  }
  material  *mp;
  double y0, y1, z0, z1, k;
};

bool yz_rect::hit(const ray& r, double t0, double t1, hit_record& rec) const {
  double t = (k-r.origin().x) / r.direction().x;
  if (t < t0 || t > t1)
    return false;
  double y = r.origin().y + t*r.direction().y;
  double z = r.origin().z + t*r.direction().z;
  if (y < y0 || y > y1 || z < z0 || z > z1)
    return false;
  rec.u = (y-y0)/(y1-y0);
  rec.v = (z-z0)/(z1-z0);
  rec.t = t;
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  rec.normal = vec4(1, 0, 0);
  return true;
}

class box: public hittable {
 public:
  box() {}
  box(const vec4& p0, const vec4& p1, material *ptr);
  virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const {
    box =  aabb(pmin, pmax);
    return true;
  }
  vec4 pmin, pmax;
  hittable *list_ptr;
};

box::box(const vec4& p0, const vec4& p1, material *ptr) {
  pmin = p0;
  pmax = p1;
  hittable **list = new hittable*[6];
  list[0] = new xy_rect(p0.x, p1.x, p0.y, p1.y, p1.z, ptr);
  list[1] = new flip_normals(
    new xy_rect(p0.x, p1.x, p0.y, p1.y, p0.z, ptr));
  list[2] = new xz_rect(p0.x, p1.x, p0.z, p1.z, p1.y, ptr);
  list[3] = new flip_normals(
    new xz_rect(p0.x, p1.x, p0.z, p1.z, p0.y, ptr));
  list[4] = new yz_rect(p0.y, p1.y, p0.z, p1.z, p1.x, ptr);
  list[5] = new flip_normals(
    new yz_rect(p0.y, p1.y, p0.z, p1.z, p0.x, ptr));
  list_ptr = new hittable_list(list,6);
}

bool box::hit(const ray& r, double t0, double t1, hit_record& rec) const {
  return list_ptr->hit(r, t0, t1, rec);
}
#endif
