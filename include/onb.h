/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __ONB_H__
#define __ONB_H__

#include "vec4.h"
class onb {
 public:
  onb() {}
  inline vec4 operator[](int i) const { return axis[i]; }
  vec4 u() const {return axis[0]; }
  vec4 v() const {return axis[1]; }
  vec4 w() const {return axis[2]; }
  vec4 local(double a, double b, double c) const { return u()*a + v()*b + w()*c; }
  vec4 local(const vec4& a) const { return u()*a.x + v()*a.y + w()*a.z; }
  void build_from_w(const vec4& n);

  vec4 axis[3];
};

void onb::build_from_w(const vec4& n) {
  axis[2] = n.normalized();
  vec4 a;
  if (fabs(w().x) > 0.9)
    a = vec4(0, 1, 0);
  else
    a = vec4(1, 0, 0);
  axis[1] = cross(w(), a).normalized();
  axis[0] = cross(w(), v());
}

#endif
