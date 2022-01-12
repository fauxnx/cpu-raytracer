/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __SPHERE_H__
#define __SPHERE_H__ 1

#include "hittable.h"
#include "onb.h"

void get_sphere_uv(const vec4& p, double& u, double& v) {
  double phi = atan2(p.z, p.x);
  double theta = asin(p.y);
  u = static_cast<double>(1-(phi + M_PI) / (2*M_PI));
  v = static_cast<double>((theta + M_PI/2) / M_PI);
}

class sphere : public hittable {
 public:
  sphere() {}
  sphere(vec4 cen, double r, material* mat) : center(cen), radius(r), mat_ptr(mat) {};
  virtual bool hit(const ray&r, double t_min, double t_max, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const;
  double pdf_value(const vec4& o, const vec4& v) const;
  vec4 random(const vec4& o) const;

  vec4 center;
  double radius;
  material *mat_ptr;
};
double sphere::pdf_value(const vec4& o, const vec4& v) const {
  hit_record rec;
  if (this->hit(ray(o, v), 0.001, DBL_MAX, rec)) {
    double cos_theta_max = sqrt(1 - radius*radius/(center-o).squared_length());
    double solid_angle = 2*double(M_PI)*(1-cos_theta_max);
    return 1 / solid_angle;
  }
  else
    return 0;
}

vec4 sphere::random(const vec4& o) const {
  vec4 direction = center - o;
  double distance_squared = direction.squared_length();
  onb uvw;
  uvw.build_from_w(direction);
  return uvw.local(random_to_sphere(radius, distance_squared));
}

bool sphere::hit(const ray&r, double t_min, double t_max, hit_record& rec) const {
  vec4 oc = r.origin() - center;
  double a = dot(r.direction(), r.direction());
  double b = dot(oc, r.direction());
  double c = dot(oc, oc) - radius*radius;
  double discriminant = b*b - a*c;
  if (discriminant > 0) {
    double temp = (-b - sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
      rec.normal =  (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
      rec.normal =  (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}
bool sphere::bounding_box(double t0, double t1, aabb& box) const {
  box = aabb(center - vec4(radius, radius, radius),
             center + vec4(radius, radius, radius));
  return true;
}

class moving_sphere : public hittable {
 public:
  moving_sphere() {}
  moving_sphere(vec4 cen0, vec4 cen1, double t0, double t1, double r, material *m)
  : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m){};
  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& box) const;
  vec4 center(double time) const;
  vec4 center0, center1;
  double time0, time1;
  double radius;
  material *mat_ptr;
};

vec4 moving_sphere::center(double time) const {
  return center0 + (center1 - center0) * ((time - time0) / (time1 - time0));
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  vec4 oc = r.origin() - center(r.time());
  double a = dot(r.direction(), r.direction());
  double b = dot(oc, r.direction());
  double c = dot(oc, oc) - radius*radius;
  double discriminant = b*b - a*c;
  if (discriminant > 0) {
    double temp = (-b - sqrt(discriminant))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_sphere_uv((rec.p-center(r.time()))/radius, rec.u, rec.v);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_sphere_uv((rec.p-center(r.time()))/radius, rec.u, rec.v);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}
bool moving_sphere::bounding_box(double t0, double t1, aabb& box) const {
  aabb box0(center(t0) - vec4(radius, radius, radius),
            center(t0) + vec4(radius, radius, radius));
  aabb box1(center(t1) - vec4(radius, radius, radius),
            center(t1) + vec4(radius, radius, radius));
  box = surrounding_box(box0, box1);
  return true;
}

#endif
