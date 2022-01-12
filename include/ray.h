/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __RAY_H__
#define __RAY_H__ 1

#include "vec4.h"

class ray {
 public:
  ray() {}
  ray(const vec4& a, const vec4& b, double ti = 0.0) { A = a; B = b; _time = ti;}
  vec4 origin() const                    { return A; }
  vec4 direction() const                 { return B; }
  double time() const                     { return _time; }
  vec4 point_at_parameter(double t) const { return A + B*t; }

  vec4 A;
  vec4 B;
  double _time;
};

#endif
