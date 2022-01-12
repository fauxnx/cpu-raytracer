/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "random.h"
#include "ray.h"

class camera {
 public:
  camera() {}
  camera(vec4 lookfrom, vec4 lookat, vec4 vup, double vfov,
         double aspect, double aperture, double focus_dist,
         double t0, double t1) {

    time0 = t0;
    time1 = t1;
    lens_radius = aperture / 2;
    double theta = vfov*double(M_PI)/180.0;
    double half_height = tan(theta/2);
    double half_width = aspect * half_height;
    origin = lookfrom;
    w = (lookfrom - lookat).normalized();
    u = (cross(vup, w)).normalized();
    v = cross(w, u);
    lower_left_corner = origin - u*half_width*focus_dist
      - v*half_height*focus_dist - w*focus_dist;
    horizontal = u * 2 * half_width * focus_dist;
    vertical = v * 2 * half_height * focus_dist;
  }

  ray get_ray(double s, double t) {
    vec4 rd = random_in_unit_disk() * lens_radius;
    vec4 offset = u * rd.x + v * rd.y;
    double time = time0 + random_double()*(time1-time0);
    return ray(origin + offset, lower_left_corner + horizontal*s + vertical*t
      - origin - offset, time);
  }

  vec4 origin;
  vec4 lower_left_corner;
  vec4 horizontal;
  vec4 vertical;
  vec4 u, v, w;
  double time0, time1;
  double lens_radius;
};

#endif
