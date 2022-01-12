/* Author: Diego Cosin <cosinma@esat-alumni.com>. */
#ifndef __RANDOM_H__
#define __RANDOM_H__ 1

#include <functional>
#include <random>
// #include <cstdlib>

// Using rand() seems to break multithreaded rendering

inline double random_double() {
  // return static_cast<double>(rand() / (RAND_MAX + 1.0));
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  static std::function<double()> rand_generator = std::bind(distribution, generator);
  return rand_generator();
}

inline vec4 random_cosine_direction() {
  double r1 = random_double();
  double r2 = random_double();
  double z = sqrt(1-r2);
  double phi = 2*double(M_PI)*r1;
  double x = cos(phi)*sqrt(r2);
  double y = sin(phi)*sqrt(r2);
  return vec4(x, y, z);
}

inline vec4 random_to_sphere(double radius, double distance_squared) {
  double r1 = random_double();
  double r2 = random_double();
  double z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);
  double phi = 2*double(M_PI)*r1;
  double x = cos(phi)*sqrt(1-z*z);
  double y = sin(phi)*sqrt(1-z*z);
  return vec4(x, y, z);
}

vec4 random_in_unit_disk() {
  vec4 p;
  do {
    p = vec4(random_double(),random_double(),0.0)*2.0 - vec4(1,1,0);
  } while (dot(p,p) >= 1.0);
  return p;
}

vec4 random_in_unit_sphere() {
  vec4 p;
  do {
    p = vec4(random_double(), random_double(), random_double())*2.0 - vec4(1,1,1);
  } while (dot(p,p) >= 1.0);//(p.squared_length() >= 1.0);
  return p;
}

vec4 random_on_unit_sphere() {
    vec4 p;
    do {
        p = vec4(random_double(),random_double(),random_double())*2.0 - vec4(1,1,1);
    } while (dot(p,p) >= 1.0);
    return p.normalized();
}
#endif
