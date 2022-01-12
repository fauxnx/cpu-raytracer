/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __MATERIAL_H__
#define __MATERIAL_H__ 1

#include "hittable.h"
#include "random.h"
#include "texture.h"
#include "onb.h"
#include "pdf.h"

struct hit_record;

bool refract(const vec4& v, const vec4& n, double ni_over_nt, vec4& refracted) {
  vec4 uv = v.normalized();
  double dt = dot(uv, n);
  double discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
  if (discriminant > 0) {
    refracted = (uv - n*dt)*ni_over_nt - n*sqrt(discriminant);
    return true;
  }
  else {
    return false;
  }
}

double schlick(double cosine, double ref_idx) {
  double r0 = (1.0-ref_idx) / (1.0+ref_idx);
  r0 = r0*r0;
  return r0 + (1.0-r0)*pow((1.0-cosine),5);
}

struct scatter_record {
    ray specular_ray;
    bool is_specular;
    vec4 attenuation;
    pdf *pdf_ptr;
};

class material {
 public:
  virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
    return false;
  }
  virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
      return 0;
  }
  virtual vec4 emitted(const ray& r_in, const hit_record& rec, double u, double v, const vec4& p) const {
    return vec4(0.0,0.0,0.0);
  }
};

class lambertian : public material {
 public:
  lambertian(texture *a) : albedo(a) {}
  double scattering_pdf(const ray& r_in, const hit_record& rec,
      const ray& scattered) const {
    double cosine = dot(rec.normal, (scattered.direction()).normalized());
    if (cosine < 0.0)
      return 0.0;
    return cosine / double(M_PI);
  }
  bool scatter(const ray& r_in, const hit_record& hrec,
    scatter_record& srec) const {
    srec.is_specular = false;
    srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
    srec.pdf_ptr = new cosine_pdf(hrec.normal);
    return true;
  }
  texture *albedo;
};

class metal : public material {
 public:
  metal(const vec4& a, double f) : albedo(a) {
    if (f < 1) fuzz = f; else fuzz = 1;
  }
  bool scatter(const ray& r_in, const hit_record& hrec,
    scatter_record& srec) const {
    vec4 reflected = reflect((r_in.direction()).normalized(), hrec.normal);
    srec.specular_ray = ray(hrec.p, reflected+random_in_unit_sphere()*fuzz);
    srec.attenuation = albedo;
    srec.is_specular = true;
    srec.pdf_ptr = 0;
    return true;
  }
  vec4 albedo;
  double fuzz;
};

class dielectric : public material {
public:
  dielectric(double ri) : ref_idx(ri) {}
  virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
    srec.is_specular = true;
    srec.pdf_ptr = 0;
    srec.attenuation = vec4(1.0, 1.0, 1.0);
    vec4 outward_normal;
    vec4 reflected = reflect(r_in.direction(), hrec.normal);
    vec4 refracted;
    double ni_over_nt;
    double reflect_prob;
    double cosine;
    if (dot(r_in.direction(), hrec.normal) > 0) {
      outward_normal = -hrec.normal;
      ni_over_nt = ref_idx;
      cosine = ref_idx * dot(r_in.direction(), hrec.normal) / r_in.direction().length();
    }
    else {
      outward_normal = hrec.normal;
      ni_over_nt = 1.0 / ref_idx;
      cosine = -dot(r_in.direction(), hrec.normal) / r_in.direction().length();
    }
    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
      reflect_prob = schlick(cosine, ref_idx);
    }
    else {
      reflect_prob = 1.0;
    }
    if (random_double() < reflect_prob) {
      srec.specular_ray = ray(hrec.p, reflected);
    }
    else {
      srec.specular_ray = ray(hrec.p, refracted);
    }
    return true;
  }

  double ref_idx;
};

class diffuse_light : public material {
 public:
  diffuse_light(texture *a) : emit(a) {}
  virtual vec4 emitted(const ray& r_in, const hit_record& rec, double u, double v, const vec4& p) const {
    if (dot(rec.normal, r_in.direction()) < 0.0)
      return emit->value(u, v, p);
    else
      return vec4(0.0,0.0,0.0);
  }
  texture *emit;
};

// class isotropic : public material {
//  public:
//   isotropic(texture *a) : albedo(a) {}
//  bool scatter(const ray& r_in, const hit_record& hrec,
//    scatter_record& srec) const { return false; }
//       scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
//       attenuation = albedo->value(rec.u, rec.v, rec.p);
//       return true;
//     }
//     texture *albedo;
// };
#endif
