/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __AABB_H__
#define __AABB_H__

#include "ray.h"

inline double ffmin(double a, double b) { return a < b ? a : b; }
inline double ffmax(double a, double b) { return a > b ? a : b; }

class aabb {
 public:
  aabb() {}
  aabb(const vec4& a, const vec4& b) { _min = a; _max = b; }

  vec4 min() const {return _min; }
  vec4 max() const {return _max; }

  // vec3 min() const { return _min; }
  // vec3 max() const { return _max; }
  //
  // bool hit(const ray& r, double t_min, double t_max) const {
  //   for (int a = 0; a < 3; ++a) {
  //     double t0 = ffmin((_min[a] - r.origin[a]) / r.direction[a],
  //                       _max[a] - r.origin[a]) / r.direction[a]);
  //     double t1 = ffmax((_min[a] - r.origin[a]) / r.direction[a],
  //                       _max[a] - r.origin[a]) / r.direction[a]);
  //     t_min = ffmax(t0, t_min);
  //     t_max = ffmin(t1, t_max);
  //     if (t_max <= t_min) {
  //       return false;
  //     }
  //   }
  //   return true;
  // }

  // Andrew Kensler's version:
  inline bool hit(const ray& r, double tmin, double tmax) const {
    for (int a = 0; a < 3; a++) {
        double invD = 1.0 / r.direction()[a];
        double t0 = (min()[a] - r.origin()[a]) * invD;
        double t1 = (max()[a] - r.origin()[a]) * invD;
        if (invD < 0.0)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin)
            return false;
    }
    return true;
  }

  vec4 _min;
  vec4 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
  vec4 small( ffmin(box0.min().x, box1.min().x),
              ffmin(box0.min().y, box1.min().y),
              ffmin(box0.min().z, box1.min().z));
  vec4 big  ( ffmax(box0.max().x, box1.max().x),
              ffmax(box0.max().y, box1.max().y),
              ffmax(box0.max().z, box1.max().z));
  return aabb(small,big);
}

#endif
